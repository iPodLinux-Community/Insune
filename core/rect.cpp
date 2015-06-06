#include "rect.h"

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)>(y))?(x):(y))

Rect::Rect(const Rect &rect)
 :xl(rect.xl), xr(rect.xr), yt(rect.yt), yb(rect.yb)
{ }
Rect::Rect(int x0, int y0, int x1, int y1)
 : xl(x0), xr(x1), yt(y0), yb(y1)
{
	xl = MIN(x0, x1);
	xr = MAX(x0, x1);
	yt = MIN(y0, y1);
	yb = MAX(y0, y1);
}
Rect::Rect(const Position &pos, const Size &s)
 : xl(pos.X()), xr(pos.X() + s.width()), yt(pos.Y()), yb(pos.Y() + s.height())
{ }

int Rect::width() const
{
	return xr - xl;
}

int Rect::height() const
{
	return yb - yt;
}

int Rect::left() const
{
	return xl;
}

int Rect::right() const
{
	return xr;
}

int Rect::top() const
{
	return yt;
}

int Rect::bottom() const
{
	return yb;
}

bool Rect::operator==(const Rect &a) const
{
	return (xl == a.xl && xr == a.xr && yt == a.yt && yb == a.yb);
}

bool Rect::operator!=(const Rect &a) const
{
	return (xl != a.xl || xr != a.xr || yt != a.yt || yb != a.yb);
}
