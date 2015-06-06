#include "size.h"

Size::Size(int w, int h)
{
	this->w = w;
	this->h = h;
}

Size::Size(const Size &size)
{
	w = size.w;
	h = size.h;
}

int Size::width() const
{
	return w;
}

int Size::height() const
{
	return h;
}

bool Size::operator==(const Size &a) const
{
	return (w == a.w && h == a.h);
}

bool Size::operator!=(const Size &a) const
{
	return (w != a.w || h != a.h);
}
