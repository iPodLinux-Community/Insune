#include <string.h>
#include "insune.h"
#include "style.h"
#include "checkbox.h"

Checkbox::Checkbox()
 : Widget(new Canvas(1,1)), checked(false)
{ }

Checkbox::~Checkbox()
{
}

void Checkbox::draw(Canvas &canvas)
{
	canvas.clear();
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;

	if ((o = style->resolve("checkbox/background")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0),getSize()));
	if (checked && (o = style->resolve("checkbox/fill")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0),getSize()));
	if ((o = style->resolve("checkbox/border")))
		canvas.drawRectangle(Rect(Position(0,0),getSize()));
}

void Checkbox::event(Event &)
{ }

void Checkbox::setChecked(bool v)
{
	checked = v;
	setDirty(true);
}

bool Checkbox::isChecked()
{
	return checked;
}
