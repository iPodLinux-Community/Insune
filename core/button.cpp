#include <string.h>
#include "insune.h"
#include "style.h"
#include "font.h"
#include "button.h"

Button::Button()
 : Widget(new Canvas(1,1)), text(0)
{ }

Button::Button(const char *t)
 : Widget(new Canvas(1,1)), text(0)
{
	setText(t);
}

Button::~Button()
{
	if (text)
		free(text);
}

void Button::draw(Canvas &canvas)
{
	canvas.clear();
	if (!text) return;

	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;
	Font *font;

	if ((o = style->resolve("button/background")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0),getSize()));

	if ((o = style->resolve("button/font")))
		font = FontFactory::getSingleton()->loadFont(o->getValue());
	else
		font = FontFactory::getSingleton()->getDefaultFont();

	Padding pad(0,0);
	if ((o = style->resolve("button/padding")))
		pad = o->getPadding();

	if ((o = style->resolve("button/border"))) {
		canvas.setColor(atoc(o->getValue()));
		canvas.drawRectangle(Rect(Position(0,0),getSize()));
	}

	if ((o = style->resolve("button/color")))
		canvas.setColor(atoc(o->getValue()));

	int minl = pad.getLeft(getSize().width());
	int mint = pad.getTop(getSize().height());
	int maxl = (getSize().width()-font->width(text))/2;
	int maxt = (getSize().height()-font->height(text))/2;
	canvas.drawText((minl>maxl)?minl:maxl, (mint>maxt)?mint:maxt, text);
}

void Button::event(Event &)
{ }

void Button::setSize(const Size &s)
{
	if (s != getSize()) {
		getCanvas()->resize(s.width(), s.height());
		Widget::setSize(s);
	}
}


void Button::setText(const char *t)
{
	if (t != text) {
		if (text)
			free(text);
		text = strdup(t);
	}

	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;
	Font *font;

	if ((o = style->resolve("button/font")))
		font = FontFactory::getSingleton()->loadFont(o->getValue());
	else
		font = FontFactory::getSingleton()->getDefaultFont();

	Padding pad(0,0);
	if ((o = style->resolve("button/padding")))
		pad = o->getPadding();

	int tw = pad.getLeft(getSize().width())
			+ font->width(text)
			+ pad.getRight(getSize().width());
	int th = pad.getTop(getSize().height()) + font->height(text)
			+ font->height(text)
			+ pad.getBottom(getSize().height());

	setSize(Size(tw, th));
	setDirty(true);
}
