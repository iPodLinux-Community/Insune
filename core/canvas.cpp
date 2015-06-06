#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "canvas.h"
#include "appearance.h"

Canvas::Canvas(uint w, uint h)
 : Drawable(w, h)
{
	tx = ty = 0;
	antialiased = false;
	font = 0;
	color = HD_CLEAR;
	background = HD_CLEAR;
	move(0, 0);
}

Canvas::~Canvas()
{
}

void Canvas::translate(int x, int y)
{
	tx = x;
	ty = y;
}

void Canvas::setBackground(uint32 bg)
{
	background = bg;
}

void Canvas::clear()
{
	HD_FillRect(surface(), 0, 0, HD_SRF_WIDTH(surface()),
		HD_SRF_HEIGHT(surface()), background);
	setDirty(true);
}

void Canvas::rotate(int deg)
{
	obj->canvas = HD_SurfaceRotate(obj->canvas, deg);
	resize(HD_SRF_WIDTH(obj->canvas), HD_SRF_HEIGHT(obj->canvas));
	setDirty(true);
}

void Canvas::setColor(Color col)
{
	color = col;
}
void Canvas::setFont(Font *font)
{
	this->font = font;
}

void Canvas::setAA(bool aa)
{
	antialiased = aa;
}

void Canvas::drawPixel(int x, int y)
{
	x += tx;
	y += ty;
	HD_Pixel(obj->canvas, x, y, color);
	setDirty(true);
}

void Canvas::drawLine(int x0, int y0, int x1, int y1)
{
	x0 += tx, y0 += ty;
	x1 += tx, y1 += ty;
	if (antialiased)
		HD_AALine(obj->canvas, x0, y0, x1, y1, color);
	else
		HD_Line(obj->canvas, x0, y0, x1, y1, color);
	setDirty(true);
}

void Canvas::translatePoints(hd_point *points, int n)
{
	hd_point *point = points;
	
	while (n--){
		point->x += tx;
		point->y += ty;
		point++;
	}
}

void Canvas::drawLines(hd_point *points, int n)
{
	translatePoints(points, n);

	if (antialiased)
		HD_AALines(obj->canvas, points, n, color);
	else
		HD_Lines(obj->canvas, points, n, color);
	setDirty(true);
}

void Canvas::drawPolygon(hd_point *points, int n)
{
	translatePoints(points, n);

	if (antialiased)
		HD_AAPoly(obj->canvas, points, n, color);
	else
		HD_Poly(obj->canvas, points, n, color);
	setDirty(true);
}

void Canvas::fillPolygon(hd_point *points, int n)
{
	translatePoints(points, n);

	HD_FillPoly(obj->canvas, points, n, color);
	setDirty(true);
}

void Canvas::drawRectangle(int x0, int y0, int x1, int y1)
{
	x0 += tx, y0 += ty;
	x1 += tx, y1 += ty;
	HD_Rect(obj->canvas, x0, y0, x1, y1, color);
	setDirty(true);
}
void Canvas::drawRectangle(const Rect &rect)
{ drawRectangle(rect.left(), rect.top(), rect.right(), rect.bottom()); }

void Canvas::fillRectangle(int x0, int y0, int x1, int y1)
{
	x0 += tx, y0 += ty;
	x1 += tx, y1 += ty;
	HD_FillRect(obj->canvas, x0, y0, x1, y1, color);
	setDirty(true);
}
void Canvas::fillRectangle(const Rect &rect)
{ fillRectangle(rect.left(), rect.top(), rect.right(), rect.bottom()); }

void Canvas::drawBitmap(int x, int y, int w, int h, const unsigned short *bits)
{
	x += tx, y += ty;
	HD_Bitmap(obj->canvas, x, y, w, h, bits, color);
	setDirty(true);
}

void Canvas::drawCircle(int x, int y, int r)
{
	x += tx, y += ty;
	if (antialiased)
		HD_AACircle(obj->canvas, x, y, r, color);
	else
		HD_Circle(obj->canvas, x, y, r, color);
	setDirty(true);
}

void Canvas::fillCircle(int x, int y, int r)
{
	x += tx, y += ty;
	if (antialiased)
		HD_AAFillCircle(obj->canvas, x, y, r, color);
	else
		HD_FillCircle(obj->canvas, x, y, r, color);
	setDirty(true);
}

void Canvas::drawEllipse(int x, int y, int rx, int ry)
{
	x += tx, y += ty;
	if (antialiased)
		HD_AAEllipse(obj->canvas, x, y, rx, ry, color);
	else
		HD_Ellipse(obj->canvas, x, y, rx, ry, color);
	setDirty(true);
}

void Canvas::fillEllipse(int x, int y, int rx, int ry)
{
	x += tx, y += ty;
	if (antialiased)
		HD_AAFillEllipse(obj->canvas, x, y, rx, ry, color);
	else
		HD_FillEllipse(obj->canvas, x, y, rx, ry, color);
	setDirty(true);
}

void Canvas::drawBezier(int order, hd_point *points, int resolution)
{
	translatePoints(points, resolution);

	if (antialiased)
		HD_AABezier(obj->canvas, order, points, resolution, color);
	else
		HD_Bezier(obj->canvas, order, points, resolution, color);
	setDirty(true);
}

void Canvas::drawText(int x, int y, const char *ch)
{
	x += tx, y += ty;
	Font *font = this->font;
	if (!font)
		font = FontFactory::getSingleton()->getDefaultFont();
	HD_Font_Draw(obj->canvas, font->getFont(), x, y, color, ch);
	setDirty(true);
}

void Canvas::blit(Drawable &canvas, const Rect &from, const Rect &to)
{
	HD_ScaleBlendClip(canvas.surface(), from.left(), from.top(),
			from.width(), from.height(),
			obj->canvas, to.left() + tx, to.top() + ty,
			to.width(), to.height(), 0, 0xff);
}


void Canvas::blit(Drawable &canvas, int x0, int y0, int x1, int y1)
{
	blit(canvas, Rect(0, 0, canvas.width(), canvas.height()),
			Rect(x0, y0, x1, y1));
}
void Canvas::blit(Drawable &canvas, const Rect &rect)
{ blit(canvas, Rect(0, 0, canvas.width(), canvas.height()), rect); }

void Canvas::drawVisual(const Visual &visual, const Rect &rect)
{
	Visual::iterator it = visual.start();
	for (; it != visual.end(); ++it)
		(*it)->render(*this, rect);
}

#include "insune.h"

DefaultCanvas::DefaultCanvas()
 : Canvas(Insune::getSingleton()->width(), Insune::getSingleton()->height()-20)
{
	move(0, 20);
}
