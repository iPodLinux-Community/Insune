#include <string.h>
#include "insune.h"
#include "style.h"
#include "radiobox.h"

Radiobox::Radiobox()
 : Widget(new Canvas(1,1)), checked(false)
{ }

Radiobox::~Radiobox()
{
}

void Radiobox::draw(Canvas &canvas)
{
	canvas.clear();
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;

	if ((o = style->resolve("radiobox/background")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0),getSize()));
	if (checked && (o = style->resolve("radiobox/fill")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0),getSize()));
	if ((o = style->resolve("radiobox/border")))
		canvas.drawRectangle(Rect(Position(0,0),getSize()));
}

void Radiobox::event(Event &)
{ }

void Radiobox::setChecked(bool v)
{
	checked = v;
	setDirty(true);
}

bool Radiobox::isChecked()
{
	return checked;
}
