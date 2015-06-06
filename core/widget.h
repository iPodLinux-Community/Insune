#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "size.h"
#include "position.h"
#include "canvas.h"
#include "engine.h"
#include "timer.h"
#include "list.h"

class Widget;

class WidgetContainer
{
 public:
	WidgetContainer();
	virtual ~WidgetContainer();

	void drawChildren();
	void updateChildren();

	virtual void addWidget(Widget *widget);
	virtual void removeWidget(Widget *widget);

	virtual bool isDirty() const;

	const list<Widget *> &getChildren();

 private:
	list<Widget *> widgets;
};

class Widget : public WidgetContainer
{
 public:
	Widget(Canvas *canvas = NULL);
	virtual ~Widget();

	virtual void draw(Canvas &canvas);
	virtual void event(Event &e) = 0;

	virtual void setSize(const Size &size);
	virtual const Size &getSize();

	virtual void setPosition(const Position &pos);
	virtual const Position &getPosition() const;
	const Position getAbsolutePosition() const;

	void setCanvas(Canvas *);
	Canvas *getCanvas() const;

	void setRelativeTo(Widget *w);

	bool hasTransition() const;
	void setTransition(bool t);

	void addTimer(Timer *);
	void removeTimer(Timer *);
	void update();

	bool isDirty() const;
	void setDirty(bool);

 private:
	Canvas *canvas;
	Size size;
	Position position;
	bool dirty;
	bool transition;
	list<Timer *> timers;
	Widget *relativeTo;
};

#endif
