#include "widget.h"
#include "insune.h"

WidgetContainer::WidgetContainer()
{ }

WidgetContainer::~WidgetContainer()
{ }

void WidgetContainer::updateChildren()
{
	list<Widget *>::iterator it = widgets.start();
	for (; it != widgets.end(); ++it) {
		(*it)->update();
		(*it)->updateChildren();
	}
}

void WidgetContainer::drawChildren()
{
	list<Widget *>::iterator it = widgets.start();
	for (; it != widgets.end(); ++it) {
		Widget *w = (*it);
		if (w->isDirty()) {
			const Position &pos = w->getAbsolutePosition();
			Canvas *canvas = w->getCanvas();
			if (canvas) {
				canvas->move(pos.X(), pos.Y());
				w->draw(*canvas);
			}
			w->drawChildren();
			w->setDirty(false);
		}
	}
}

const list<Widget *> &WidgetContainer::getChildren()
{
	return widgets;
}

void WidgetContainer::addWidget(Widget *w)
{
	widgets.append(w);
}

void WidgetContainer::removeWidget(Widget *w)
{
	widgets.remove(w);
}

bool WidgetContainer::isDirty() const
{
	list<Widget *>::iterator it = widgets.start();
	for (; it != widgets.end(); ++it) {
		Widget *w = (*it);
		if (w->isDirty())
			return true;
	}
	return false;
}

Widget::Widget(Canvas *c)
 : canvas(c), size(0, 0), position(0, 0), dirty(true), transition(true), relativeTo(NULL)
{
	if (canvas) {
		setSize(Size(canvas->width(), canvas->height()));
		setPosition(Position(c->x(), c->y()));
	}
}

Widget::~Widget()
{
	setCanvas(NULL);
}

void Widget::setSize(const Size &size)
{
	this->size = size;
	if (canvas)
		canvas->resize(size.width(), size.height());
}

const Size &Widget::getSize()
{
	if (canvas)
		size = Size(canvas->width(), canvas->height());
	return size;
}

void Widget::setRelativeTo(Widget *w)
{
	relativeTo = w;
}

void Widget::setPosition(const Position &pos)
{
	position = pos;
}

const Position Widget::getAbsolutePosition() const
{
	if (relativeTo)
		return getPosition() + relativeTo->getAbsolutePosition();
	else
		return getPosition();
}

const Position &Widget::getPosition() const
{
	return position;
}

bool Widget::isDirty() const
{
	if (WidgetContainer::isDirty())
		return true;
	return dirty;
}

void Widget::setDirty(bool d)
{
	dirty = d;
}

void Widget::setCanvas(Canvas *c)
{
	if (canvas)
		delete canvas;
	canvas = c;
}

Canvas *Widget::getCanvas() const
{
	return canvas;
}

bool Widget::hasTransition() const
{
	return transition;
}

void Widget::setTransition(bool t)
{
	transition = t;
}

void Widget::addTimer(Timer *timer)
{
	timers.append(timer);
}

void Widget::removeTimer(Timer *timer)
{
	timers.remove(timer);
}

void Widget::update()
{
	list<Timer *>::iterator it = timers.start();
	while (it != timers.end()) {
		Timer *timer = (*it); ++it;
		if (timer->isTime()) {
			timer->performAction();
			if (timer->isSingleUse()) {
				timers.remove(timer);
				delete timer;
			}
		}
	}
}
void Widget::draw(Canvas &canvas)
{ }
