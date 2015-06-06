#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "exception.h"
#include "drawable.h"

Drawable::Drawable(uint w, uint h)
 : surfaceOwner(true)
{
	obj = HD_New_Object();
	obj->type = HD_TYPE_CANVAS;
	obj->canvas = HD_NewSurface(w, h);
	obj->render = HD_Canvas_Render;
	obj->destroy = HD_Canvas_Destroy;
	obj->natw = obj->w = w;
	obj->nath = obj->h = h;
	setDirty(true);
}
Drawable::Drawable(hd_object *object)
 : surfaceOwner(false)
{
	obj = HD_New_Object();
	obj->type = HD_TYPE_CANVAS;
	obj->canvas = object->canvas;
	obj->render = HD_Canvas_Render;
	obj->destroy = HD_Canvas_Destroy;
	obj->natw = obj->w = object->w;
	obj->nath = obj->h = object->h;

	setDirty(true);
}

Drawable::~Drawable()
{
	if (surfaceOwner) {
		free(obj->canvas);
	}
	free(obj);
}

int Drawable::x() const
{
	return obj->x;
}

int Drawable::y() const
{
	return obj->y;
}

uint Drawable::width() const
{
	return obj->w;
}

uint Drawable::height() const
{
	return obj->h;
}

void Drawable::setDirty(bool t)
{
	obj->dirty = (int)t;
}

void Drawable::move(int x, int y)
{
	obj->x = x;
	obj->y = y;
}

void Drawable::resize(uint width, uint height)
{
	HD_FreeSurface(obj->canvas);
	obj->canvas = HD_NewSurface(width, height);
	obj->natw = obj->w = width;
	obj->nath = obj->h = height;
	setDirty(true);
}


#define APREC 15
#define ZPREC 8

#define RGBA_SPLIT(src, r,g,b,a) \
do { \
	(a) = ((src) >> 24);        \
	(r) = ((src) >> 16) & 0xff; \
	(g) = ((src) >>  8) & 0xff; \
	(b) = ((src)      ) & 0xff; \
} while (0)

static inline void blurInner(uint32 *buf, int *zR, int *zG, int *zB, int *zA, int alpha)
{
	int r, g, b, a;
	RGBA_SPLIT(*buf, r, g, b, a);

	*zR += (alpha * ((r << ZPREC) - *zR)) >> APREC;
	*zG += (alpha * ((g << ZPREC) - *zG)) >> APREC;
	*zB += (alpha * ((b << ZPREC) - *zB)) >> APREC;
	*zA += (alpha * ((a << ZPREC) - *zA)) >> APREC;

	*buf = HD_RGBA(*zR >> ZPREC, *zG >> ZPREC, *zB >> ZPREC, *zA >> ZPREC);
}

static inline void blurRow(hd_surface srf, int row, int alpha)
{
	int zR, zG, zB, zA;
	unsigned int i;
	uint32 *pix = HD_SRF_ROWF(srf, row);

	RGBA_SPLIT(*pix, zR, zG, zB, zA);
	zR <<= ZPREC;
	zG <<= ZPREC;
	zB <<= ZPREC;
	zA <<= ZPREC;

	for (i = 1; i < HD_SRF_WIDTH(srf); ++i)
		blurInner(pix + i, &zR, &zG, &zB, &zA, alpha);
	for (i = HD_SRF_WIDTH(srf) - 2; (int)i >= 0; --i)
		blurInner(pix + i, &zR, &zG, &zB, &zA, alpha);
}

static inline void blurCol(hd_surface srf, int col, int alpha)
{
	int zR, zG, zB, zA;
	unsigned int i;
	uint32 *pix = HD_SRF_PIXELS(srf);
	pix += col;

	RGBA_SPLIT(*pix, zR, zG, zB, zA);
	zR <<= ZPREC;
	zG <<= ZPREC;
	zB <<= ZPREC;
	zA <<= ZPREC;

	for (i = HD_SRF_WIDTH(srf); i < (HD_SRF_HEIGHT(srf) - 1)*HD_SRF_WIDTH(srf); i+=HD_SRF_WIDTH(srf))
		blurInner(pix + i, &zR, &zG, &zB, &zA, alpha);
	for (i = (HD_SRF_HEIGHT(srf) - 2)*HD_SRF_WIDTH(srf); (int)i >= 0; i-=HD_SRF_WIDTH(srf))
		blurInner(pix + i, &zR, &zG, &zB, &zA, alpha);
}

void expBlur(hd_surface srf, int radius)
{
	unsigned int row, col;
	int alpha;
	if (radius < 1)
		return;
	
	alpha = (int)((1 << APREC) * (1.0f - exp(-2.3 / (radius + 1.0f))));
	for (row = 0; row < HD_SRF_HEIGHT(srf); ++row)
		blurRow(srf, row, alpha);
	for (col = 0; col < HD_SRF_WIDTH(srf); ++col)
		blurCol(srf, col, alpha);
}

static inline uint linearInterpolate(int a, int b, float x)
{
	return (uint)(a * (1 - x) + b * x);
}

static inline uint linearPixel(uint o, uint t, float x)
{
	uint a, r, g, b;
	a = linearInterpolate(((o           ) >> 24),((t           ) >> 24), x);
	r = linearInterpolate(((o & 0xff0000) >> 16),((t & 0xff0000) >> 16), x);
	g = linearInterpolate(((o & 0x00ff00) >>  8),((t & 0x00ff00) >>  8), x);
	b = linearInterpolate(((o & 0x0000ff)      ),((t & 0x0000ff)      ), x);
	uint ret = (a << 24) | (r << 16) | (g << 8) | (b    );
	return ret;

}

static inline uint bilinearInterpolate(hd_surface srf, float oxf, float oyf)
{
	int ox = (int)oxf;
	int oy = (int)oyf;
	oxf -= ox;
	oyf -= oy;
	uint a,b;
	a = linearPixel(HD_SRF_GETPIX(srf, ox    , oy    ),
			HD_SRF_GETPIX(srf, ox + 1, oy + 1), oxf);
	b = linearPixel(HD_SRF_GETPIX(srf, ox    , oy + 1),
			HD_SRF_GETPIX(srf, ox + 1, oy + 1), oxf);
	return linearPixel(a, b, oyf);

}

void Drawable::scale(uint width, uint height)
{
	hd_surface srf = HD_NewSurface(width, height);
	expBlur(obj->canvas, (int)((float)obj->w/(2*width) + 0.5f) - 1);

	float px = (float)obj->w / width;
	float py = (float)obj->h / height;
	float ox, oy;

	for (uint y = 0; y < height; ++y) {
		oy = py * y;
		for (uint x = 0; x < width; ++x) {
			ox = px * x;
			uint pix = bilinearInterpolate(obj->canvas, ox, oy);
			BLEND_ARGB8888_ON_ARGB8888(HD_SRF_PIXF(srf,x,y),pix,0xff);
		}
	}
	HD_FreeSurface(obj->canvas);
	obj->canvas = srf;
	obj->natw = obj->w = width;
	obj->nath = obj->h = height;
	setDirty(true);
}

hd_surface Drawable::surface() const
{
	return obj->canvas;
}

hd_object *Drawable::object() const
{
	return obj;
}
