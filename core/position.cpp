#include "position.h"

Position::Position(int x, int y)
{
	this->x = x;
	this->y = y;
}

Position::Position(const Position &size)
{
	x = size.x;
	y = size.y;
}

int Position::X() const
{
	return x;
}

int Position::Y() const
{
	return y;
}

bool Position::operator==(const Position &a) const
{
	return (x == a.x && y == a.y);
}

bool Position::operator!=(const Position &a) const
{
	return (x != a.x || y != a.y);
}

const Position Position::operator+(const Position &r) const
{
	return Position(x + r.x, y + r.y);
}

const Position Position::operator-(const Position &r) const
{
	return Position(x - r.x, y - r.y);
}
