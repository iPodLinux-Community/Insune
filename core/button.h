#ifndef _BUTTON_H_
#define _BUTTON_H_
#include "widget.h"

class Button : public Widget
{
 public:
	Button();
	Button(const char *text);
	~Button();

	void draw(Canvas &canvas);
	void event(Event &e);

	void setSize(const Size &s);
	void setText(const char *text);

 private:
	char *text;
};

#endif
