#include "appearance.h"
#include <string.h>

Appearance::Appearance()
 : padding(0, 0), paddingSet(false)
{ }
Appearance::~Appearance()
{ }

void Appearance::setPadding(const Padding &padding)
{
	paddingSet = true;
	this->padding = padding;
}

void Appearance::render(Canvas &canvas, const Rect &rect)
{
	if (!paddingSet) {
		draw(canvas, rect);
		return;
	}
	Rect nrect = Rect(rect.left() + padding.getLeft(rect.width()),
			rect.top() + padding.getTop(rect.height()),
			rect.right() - padding.getRight(rect.width()),
			rect.bottom() - padding.getBottom(rect.height()));
	draw(canvas, nrect);
}

//void Appearance::draw(Canvas &canvas, const Rect &rect)
//{ }

SolidAppearance::SolidAppearance(Color col)
{
	color = col;
}

void SolidAppearance::draw(Canvas &canvas, const Rect &rect)
{
	canvas.setColor(color);
	canvas.fillRectangle(rect);
}


ImageAppearance::ImageAppearance(const char *file, const char *align,
	const char *type)
{
	canvas = Image::load(file);

	alignment = (Alignment)0;
	if (align) {
		char *left, *top, *bottom, *right;
		left = strstr(align, "left");
		right = left ? 0 : strstr(align, "right");
		bottom = strstr(align, "bottom");
		top = bottom ? 0 : strstr(align, "top");
		if (left) alignment = (Alignment)(alignment | ALIGN_LEFT);
		if (right) alignment = (Alignment)(alignment | ALIGN_RIGHT);
		if (top) alignment = (Alignment)(alignment | ALIGN_TOP);
		if (bottom) alignment = (Alignment)(alignment | ALIGN_BOTTOM);
	}
	
	this->type = TYPE_NORMAL;
	if (type) {
		if (!strcmp(type, "fit"))
			this->type = TYPE_FIT;
		else if (!strcmp(type, "scale"))
			this->type = TYPE_SCALE;
		else if (!strcmp(type, "repeat"))
			this->type = TYPE_REPEAT;
		else if (!strcmp(type, "repeat-x"))
			this->type = TYPE_REPEATX;
		else if (!strcmp(type, "repeat-y"))
			this->type = TYPE_REPEATY;
	}
}

ImageAppearance::~ImageAppearance()
{
	delete canvas;
}

void ImageAppearance::drawScale(Canvas &canvas, const Rect &rect)
{
	canvas.blit(*this->canvas, rect);
}

void ImageAppearance::drawRepeat(Canvas &canvas, const Rect &rect)
{
	int cw = this->canvas->width();
	int ch = this->canvas->height();
	int xi = 1, yi = 1;
	int x = 0, y = 0;
	if (type != TYPE_REPEATY) {
		while (xi * cw < rect.width()) ++xi;
		if (!(alignment & ALIGN_LEFT)) {
			if (alignment & ALIGN_RIGHT)
				x = xi * cw - rect.width();
			else
				x = (xi * cw - rect.width()) / 2;
		}
	}
	if (type != TYPE_REPEATX) {
		while (yi * ch < rect.height()) ++yi;
		if (!(alignment & ALIGN_TOP)) {
			if (alignment & ALIGN_BOTTOM)
				y = yi * ch - rect.height();
			else
				y = (yi * ch - rect.height()) / 2;
		}
	}
	for (int i = 0; i < xi; ++i) {
		for (int j = 0; j < yi; ++j) {
			int mx = x + rect.left() + cw * i;
			int my = y + rect.top() + ch * j;
			int ccw = cw;
			int cch = ch;
			if (mx + ccw > rect.right())
				ccw = rect.right() - mx;
			if (my + cch > rect.bottom())
				cch = rect.bottom() - my;
			canvas.blit(*this->canvas,
					Rect(0, 0, ccw, cch),
					Rect(mx, my, mx + ccw, my + cch));
		}
	}
}

void ImageAppearance::drawFit(Canvas &canvas, const Rect &rect)
{
	int x = 0, y = 0;
	int fw = rect.width();
	int fh = rect.height();
	if ((int)this->canvas->width() < fw)
		fw = this->canvas->width();
	else {
		if (alignment & ALIGN_RIGHT)
			x = this->canvas->width() - rect.width();
		else if (alignment & ALIGN_CENTER)
			x = (this->canvas->width() - rect.width()) / 2;
	}
	if ((int)this->canvas->height() < fh)
		fh = this->canvas->height();
	else {
		if (alignment & ALIGN_BOTTOM)
			y = this->canvas->height() - rect.height();
		else if (alignment & ALIGN_CENTER)
			y = (this->canvas->height() - rect.height()) / 2;
	}

	canvas.blit(*this->canvas, Rect(x, y, x + fw, y + fh), rect);
}

void ImageAppearance::drawNormal(Canvas &cv, const Rect &rect)
{
//FIXME: clip properly
	int x = rect.left(), y = rect.top();
	if (alignment & ALIGN_RIGHT)
		x = rect.right() - canvas->width();
	else if (alignment & ALIGN_CENTER)
		x = rect.left() + (rect.width() - canvas->width())/2;
	if (alignment & ALIGN_BOTTOM)
		y = rect.bottom() - canvas->height();
	else if (alignment & ALIGN_CENTER)
		x = rect.top() + (rect.height() - canvas->height())/2;
	cv.blit(*this->canvas, Rect(x, y,
			x + canvas->width(), y + canvas->height()));
}

void ImageAppearance::draw(Canvas &canvas, const Rect &rect)
{
	switch (type) {
	case TYPE_NORMAL:
		drawNormal(canvas, rect);
		break;
	case TYPE_SCALE:
		drawScale(canvas, rect);
		break;
	case TYPE_REPEAT:
	case TYPE_REPEATX:
	case TYPE_REPEATY:
		drawRepeat(canvas, rect);
		break;
	case TYPE_FIT:
		drawFit(canvas, rect);
		break;
	}
}

