#ifndef _LABEL_H_
#define _LABEL_H_
#include "widget.h"

class Label : public Widget
{
 public:
	Label();
	Label(const char *text);
	~Label();

	enum Alignment {
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	void draw(Canvas &canvas);
	void event(Event &e);

	void setColor(Color c);
	void setFont(Font *f);
	void setText(const char *text);
	void setAlignment(Alignment a);

	void setSize(const Size &s);
	void setScroll(bool);

	void scroll(int);

 private:
	char *text;
	Font *font;
	Color col;
	Alignment alignment;
	Timer *timer;
	int offset;
	int direction;
	bool canScroll;
};

#endif
