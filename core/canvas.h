#ifndef _CANVAS_H_
#define _CANVAS_H_
#include "drawable.h"
#include "visual.h"
#include "rect.h"
#include "font.h"

typedef uint32 Color;

class Canvas : public Drawable
{
 public:
 	Canvas(uint w, uint h);
	virtual ~Canvas();

	void rotate(int deg);
	void setBackground(uint32);
	void clear();
	void translate(int x, int y);

	void setColor(Color col);
	void setFont(Font *font);
	void setAA(bool aa);
	void drawPixel(int x, int y);
	void drawLine(int x0, int y0, int x1, int y1);
	void drawLines(hd_point *points, int n);
	void drawPolygon(hd_point *points, int n);
	void fillPolygon(hd_point *points, int n);
	void drawRectangle(int x0, int y0, int x1, int y1);
	void drawRectangle(const Rect &rect);
	void fillRectangle(int x0, int y0, int x1, int y1);
	void fillRectangle(const Rect &rect);
	void drawBitmap(int x, int y, int w, int h, const unsigned short *bits);
	void drawCircle(int x, int y, int r);
	void fillCircle(int x, int y, int r);
	void drawEllipse(int x, int y, int rx, int ry);
	void fillEllipse(int x, int y, int rx, int ry);
	void drawBezier(int o, hd_point *points, int res);
	void drawText(int x, int y, const char *ch);
	void blit(Drawable &dr, int x0, int y0, int x1, int y1);
	void blit(Drawable &dr, const Rect &rect);
	void blit(Drawable &dr, const Rect &from, const Rect &rect);
	void drawVisual(const Visual &visual, const Rect &rect);

 protected:
	void translatePoints(hd_point *points, int n);

	uint32 background;

	bool antialiased;
	Color color;
	Font *font;
 	int tx, ty;
};

// canvas for insune applications
class DefaultCanvas : public Canvas
{
 public:
	DefaultCanvas();
};

#endif
