#include <string.h>
#include "insune.h"
#include "style.h"
#include "frame.h"

Frame::Frame()
 : Widget(new Canvas(1,1))
{ }

Frame::~Frame()
{
}

void Frame::draw(Canvas &canvas)
{
	canvas.clear();
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;

	if ((o = style->resolve("frame/background")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0),getSize()));
	if ((o = style->resolve("frame/border"))) {
		canvas.setColor(atoc(o->getValue()));
		canvas.drawRectangle(Rect(Position(0,0),getSize()));
	}
}

void Frame::setSize(const Size &s)
{
	if (s != getSize()) {
		getCanvas()->resize(s.width(), s.height());
		Widget::setSize(s);
	}
}

void Frame::addWidget(const char *name, Widget *w)
{
	char buf[2048];
	sprintf(buf, "%s/geometry", name);
	WidgetContainer::addWidget(w);
	w->setRelativeTo(this);
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;

	if ((o = style->resolve(buf))) {
		Rect r = o->getGeometry(Rect(Position(0,0), getSize()));
		if (r.width() > 0 && r.height() > 0)
			w->setSize(Size(r.width(), r.height()));
		else if (r.width() > 0)
			w->setSize(Size(w->getSize().width(), r.width()));
		else if (r.height() > 0)
			w->setSize(Size(r.width(), w->getSize().width()));
		w->setPosition(Position(r.left(), r.top()));
	}
}

void Frame::event(Event &)
{ }
