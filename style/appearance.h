#ifndef _APPEARANCE_H_
#define _APPEARANCE_H_

#include "canvas.h"
#include "image.h"
#include "rect.h"
#include "padding.h"

#define GRAD_VERT 1
#define GRAD_HORZ 2

class Appearance
{
 public:
 	Appearance();
 	virtual ~Appearance();
	void setPadding(const Padding &padding);
	void render(Canvas &canvas, const Rect &rect);
	virtual void draw(Canvas &canvas, const Rect &rect) = 0;
 private:
	Padding padding;
	bool paddingSet;
};

class GradientAppearance : public Appearance
{
 public:
	GradientAppearance(int direction);

	void addColor(Color color);
	void draw(Canvas &canvas, const Rect &rect);
 private:

	void drawGradient(Canvas &canvas, Color start,
		Color end, const Rect &rect);
	int direction;
	list<Color> colors;
};

class SolidAppearance : public Appearance
{
 public:
	SolidAppearance(Color col);

	void draw(Canvas &canvas, const Rect &rect);
 private:
	Color color;
};

class ImageAppearance : public Appearance
{
 public:
	ImageAppearance(const char * file, const char * alignment = NULL,
			const char * type = NULL);
	~ImageAppearance();

	void draw(Canvas &canvas, const Rect &rect);
 private:
	void drawScale(Canvas &canvas, const Rect &rect);
	void drawFit(Canvas &canvas, const Rect &rect);
	void drawRepeat(Canvas &canvas, const Rect &rect);
	void drawNormal(Canvas &canvas, const Rect &rect);
	enum Alignment {
		ALIGN_LEFT =   (1 << 0),
		ALIGN_RIGHT =  (1 << 1),
		ALIGN_TOP =    (1 << 2),
		ALIGN_CENTER = (1 << 3),
		ALIGN_BOTTOM = (1 << 4)
	} alignment;

	enum {
		TYPE_NORMAL,
		TYPE_FIT,
		TYPE_SCALE,
		TYPE_REPEAT,
		TYPE_REPEATX,
		TYPE_REPEATY
	} type;

	Image *canvas;
};

#endif
