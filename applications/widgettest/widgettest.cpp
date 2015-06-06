#include "insune.h"
#include "label.h"
#include "button.h"
#include "frame.h"

class WidgetTestFrame : public Frame
{
 public:
	WidgetTestFrame()
	{
		Insune *insune = Insune::getSingleton();
		const Style *style = insune->getStyle();
		Option *o;

		if ((o = style->resolve("test/geometry"))) {
			Rect r = o->getGeometry(Rect(Position(0,0),
				Size(insune->width(), insune->height())));
			setSize(Size(r.width(), r.height()));
			setPosition(Position(r.left(), r.top()));
		} else {
			setSize(Size(150, 100));
			setPosition(Position((insune->width()-150)/2,
					(insune->height()-100)/2));
		}
		Label *label = new Label("Does this look ok?");
		Button *b1 = new Button("Yes!");
		Button *b2 = new Button("No!");
		label->setColor(0xffffffff);
		label->setScroll(false);
		addWidget("test/label", label);
		addWidget("test/button!accept", b1);
		addWidget("test/button!deny", b2);
	}
};

class WidgetTest : public Application
{
 public:
	WidgetTest()
	{
		addWidget(new WidgetTestFrame);
	}

	void event(Event &)
	{ }

 private:
};

APPLICATION(WidgetTest)
