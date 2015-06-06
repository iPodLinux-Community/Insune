#ifndef _RADIOBOX_H_
#define _RADIOBOX_H_
#include "widget.h"

class Radiobox : public Widget
{
 public:
	Radiobox();
	~Radiobox();

	void draw(Canvas &canvas);
	void event(Event &e);

	bool isChecked();
	void setChecked(bool);

 private:
	bool checked;
};

#endif
