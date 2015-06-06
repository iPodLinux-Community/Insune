#ifndef _PADDING_H_
#define _PADDING_H_

#define PERPAD_TOP   0x10
#define PERPAD_BOT   0x20
#define PERPAD_LEFT  0x40
#define PERPAD_RIGHT 0x80

class Padding
{
 public:
	Padding(int top, int right, int bottom, int left);
	Padding(int tb, int lr);
	Padding(int mask, int top, int right, int bottom, int left);

	int getRight(int total = 0);
	int getLeft(int total = 0);
	int getTop(int total = 0);
	int getBottom(int total = 0);

 private:
	int mask;
	int top;
	int left;
	int right;
	int bottom;
};

#endif
