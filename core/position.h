#ifndef _POSITION_H_
#define _POSITION_H_

class Position
{
 public:
	Position(int x, int y);
	Position(const Position &pos);

	int X() const;
	int Y() const;

	bool operator==(const Position &a) const;
	bool operator!=(const Position &a) const;
	const Position operator+(const Position &r) const;
	const Position operator-(const Position &r) const;

 private:
	int x;
	int y;
};

#endif
