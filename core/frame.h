#ifndef _FRAME_H_
#define _FRAME_H_
#include "widget.h"

class Frame : public Widget
{
 public:
	Frame();
	~Frame();

	void draw(Canvas &canvas);
	void event(Event &e);

	void addWidget(const char *name, Widget *w);
	void setSize(const Size &s);
};

#endif
