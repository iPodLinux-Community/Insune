#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_
#include "widget.h"

class Checkbox : public Widget
{
 public:
	Checkbox();
	~Checkbox();

	void draw(Canvas &canvas);
	void event(Event &e);

	bool isChecked();
	void setChecked(bool);

 private:
	bool checked;
};

#endif
