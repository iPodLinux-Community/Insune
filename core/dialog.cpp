#include "insune.h"
#include "dialog.h"
#include "option.h"
#include "style.h"
#include <string.h>

DialogWidget::DialogWidget(const char *message, DialogType type)
{
	Insune *insune = Insune::getSingleton();
	this->message = strdup(message);
	this->type = type;
	Font *font = FontFactory::getSingleton()->getDefaultFont();
	setSize(Size(font->width(message) + 10 * 2, font->height(message) + 10 * 2));
	setTransition(false);
	setCanvas(new Canvas(getSize().width(), getSize().height()));
	setPosition(Position((insune->width() - getSize().width())/2,
			(insune->height() - getSize().height())/2));
}

DialogWidget::~DialogWidget()
{
	free(message);
}

DialogResult DialogWidget::getResult() const
{
	return result;
}

void DialogWidget::event(Event &e)
{
	
}

void DialogWidget::draw(Canvas &canvas)
{
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;
	if ((o = style->resolve("dialog/background")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0),getSize()));
	if ((o = style->resolve("dialog/border"))) {
		canvas.setColor(atoc(o->getValue()));
		canvas.drawRectangle(Rect(Position(0,0),getSize()));
	}

	if ((o = style->resolve("dialog/color")))
		canvas.setColor(atoc(o->getValue()));
	canvas.drawText(10, 10, message);
}

Dialog::Dialog(const char *message, DialogType type)
{
	widget = new DialogWidget(message, type);
	addWidget(widget);
}

Dialog::~Dialog()
{
	removeWidget(widget);
	delete widget;
}

DialogResult Dialog::getResult() const
{
	return widget->getResult();
}
