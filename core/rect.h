#ifndef _RECT_H_
#define _RECT_H_

#include "size.h"
#include "position.h"

class Rect
{
 public:
	Rect(const Rect &rect);
	Rect(int x0, int y0, int x1, int y1);
	Rect(const Position &pos, const Size &size);

	int width() const;
	int height() const;
	int left() const;
	int right() const;
	int top() const;
	int bottom() const;

	bool operator==(const Rect &a) const;
	bool operator!=(const Rect &a) const;

 private:
	int xl;
	int xr;
	int yt;
	int yb;
};

#endif
