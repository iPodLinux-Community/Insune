#include "appearance.h"
#include "list.h"

class Gradient
{
 public:
	Gradient(Color start, Color end)
	{
		this->start = start;
		this->end = end;
		
		int x, s;
		int aA, rA, gA, bA;
		int aZ, rZ, gZ, bZ;

		aA = (start >> 24);
		rA = (start >> 16) & 0xff;
		gA = (start >>  8) & 0xff;
		bA = (start      ) & 0xff;

		aZ = (end >> 24);
		rZ = (end >> 16) & 0xff;
		gZ = (end >>  8) & 0xff;
		bZ = (end      ) & 0xff;

		if (aZ + aA < 510) {
			int max = aA, inc = 0;
			if (aZ > max) max = aZ;
			for (x = 0; x < 256; ++x) {
				inc = x * 100 / 255 * max / 100;
				s = max - inc;
				range[x] = max << 24 |
					(((rA*s)>>8)+((rZ*inc)>>8))<<16 |
					(((gA*s)>>8)+((gZ*inc)>>8))<< 8 |
					(((bA*s)>>8)+((bZ*inc)>>8));
			}
		}
		else for (x = 0; x < 256; ++x) {
			s = 255 - x;
			range[x] = 0xff000000 |
				(((rA*s)>>8)+((rZ*x)>>8))<<16 |
				(((gA*s)>>8)+((gZ*x)>>8))<< 8 |
				(((bA*s)>>8)+((bZ*x)>>8));
				
		}
	}

	bool isMatch(Color start, Color end)
	{
		return (this->start == start && this->end == end);
	}

	Color getIndex(unsigned char idx)
	{
		return range[idx];
	}

 private:
	Color start;
	Color end;
	Color range[256];
};

class GradientList : public list<Gradient *>
{
 public:
	GradientList()
	{ }
	~GradientList()
	{
		iterator it = start();
		for (; it != end(); ++it)
			delete (*it);
		destroy();
	}

	Gradient *getGradient(Color st, Color e)
	{
		iterator it = start();
		for (; it != end(); ++it) {
			if ((*it)->isMatch(st, e))
				return (*it);
		}

		Gradient *ret = new Gradient(st, e);
		append(ret);

		return ret;
	}
};

static GradientList gradients;

GradientAppearance::GradientAppearance(int direction)
{
	this->direction = direction;
}

void GradientAppearance::addColor(Color color)
{
	this->colors.append(color);
}

void GradientAppearance::drawGradient(Canvas &canvas,
	Color start, Color end, const Rect &rect)
{
	int steps, line;
	bool vert = (direction & GRAD_VERT);

	Gradient *g = gradients.getGradient(start, end);

	steps = vert ? rect.height() : rect.width();
	for (line = 0; vert && line < steps; ++line) {
		canvas.setColor(g->getIndex((line<<8)/steps));
		canvas.drawLine(rect.left(), rect.top()+line,
				rect.right()-1, rect.top()+line);
	}
	for (line = 0; !vert && line < steps; ++line) {
		canvas.setColor(g->getIndex((line<<8)/steps));
		canvas.drawLine(rect.left()+line, rect.top(),
				rect.left()+line,rect.bottom()-1);
	}
}

#include "stdio.h"
void GradientAppearance::draw(Canvas &canvas, const Rect &rect)
{
	int count = colors.count();

	if(!count)
		return;
	else if (count == 2)
		drawGradient(canvas, colors.first(), colors.last(), rect);
	else if (count == 3) {
		if (direction & GRAD_VERT) {
			Rect trect = Rect(rect.left(), rect.top(),
				rect.right(),
				rect.top() + rect.height()/2);
			drawGradient(canvas, colors.first(), colors.nth(1), trect);
			Rect brect = Rect(rect.left(),
				rect.top() + rect.height()/2,
				rect.right(), rect.bottom());
			drawGradient(canvas, colors.nth(1), colors.last(), brect);
		}
		else {
			Rect lrect = Rect(rect.left(), rect.top(),
				rect.left() + rect.width()/2,
				rect.bottom());
			drawGradient(canvas, colors.first(), colors.nth(1), lrect);
			Rect rrect = Rect(rect.left() + rect.width()/2,
				rect.top(), rect.right(), rect.bottom());
			drawGradient(canvas, colors.nth(1), colors.last(), rrect);
		}
	}
	else if (count == 1) {
		canvas.setColor(colors.first());
		canvas.fillRectangle(rect);
	}
	else {
		double step, stepd;
		int i;

		if (direction & GRAD_VERT) {
			step = (double)rect.height() / --count;

			for (i = 0, stepd = 0; i < count; i++){
				if(i == count - 1) step = rect.height() - stepd;
				else stepd += step;

				Rect crect = Rect(rect.left(), rect.top() + int(i * step),
				rect.right(), rect.top() + int((i + 1) * step));

				drawGradient(canvas, colors.nth(i), colors.nth(i + 1), crect);
			}
		}
		else {
			step = (double)rect.width() / --count;

			for (i = 0, stepd = 0; i < count; i++){
				if(i == count - 1) step = rect.width() - stepd;
				else stepd += step;

				Rect crect = Rect(rect.left() + int(i * step), rect.top(),
				rect.left() + int((i + 1) * step), rect.bottom());

				drawGradient(canvas, colors.nth(i), colors.nth(i + 1), crect);
			}
		}
	}
}


