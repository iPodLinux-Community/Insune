#include "insune.h"

class BGWidget : public Widget
{
 public:
	BGWidget(const Size &size)
	{
		setSize(size);
	}

	void draw(Canvas &canvas)
	{
		const Style *style = Insune::getSingleton()->getStyle();
		Option *o;
		if ((o = style->resolve("header/background")))
			canvas.drawVisual(o->getVisual(), Rect(Position(0,0),
								getSize()));
	}

	void event(Event &) { }

};

class Header : public Application
{
 public:
	Header()
	{
		Canvas *canvas = new Canvas(Insune::getSingleton()->width(),20);
		widget = new BGWidget(Size(canvas->width(), canvas->height()));
		widget->setCanvas(canvas);
		addWidget(widget);
		
	}

	void event(Event &)
	{ }

 private:
	BGWidget *widget;
};

APPLICATION(Header)
