#include <string.h>
#include "label.h"

Label::Label()
 : Widget(new Canvas(1,1)), text(0), font(0), alignment(ALIGN_LEFT),
 	timer(0), offset(0), direction(0), canScroll(false)
{ }

Label::Label(const char *t)
 : Widget(new Canvas(1,1)), text(0), font(0), alignment(ALIGN_LEFT),
 	timer(0), offset(0), direction(0), canScroll(false)
{
	setText(t);
}

Label::~Label()
{
	if (timer) {
		removeTimer(timer);
		delete (Action<Label, int> *)timer->getAction();
		delete timer;
	}
	if (text)
		free(text);
}

void Label::draw(Canvas &canvas)
{
	canvas.clear();
	if (!text) return;
	if (font)
		canvas.setFont(font);
	if (!font)
		font = FontFactory::getSingleton()->getDefaultFont();
	canvas.setColor(col);
	if (direction != 0) {
		canvas.drawText(offset,0,text);
		return;
	}
	switch (alignment) {
	case ALIGN_LEFT:
		canvas.drawText(0,0,text);
		break;
	case ALIGN_RIGHT:
		canvas.drawText(canvas.width()-font->width(text),0,text);
		break;
	case ALIGN_CENTER:
		canvas.drawText((canvas.width()-font->width(text))/2,0,text);
		break;
	}
}

void Label::event(Event &)
{ }

void Label::setScroll(bool s)
{
	canScroll = s;
	if (text)
		setText(text);
}

void Label::setColor(Color c)
{
	col = c;
	setDirty(true);
}

void Label::setFont(Font *f)
{
	font = f;
	if (text)
		setText(text);
}

void Label::setSize(const Size &s)
{
	if (s != getSize()) {
		getCanvas()->resize(s.width(), s.height());
		Widget::setSize(s);
	}
	if (text)
		setText(text);
}

void Label::scroll(int)
{
	if (!font)
		font = FontFactory::getSingleton()->getDefaultFont();
	if (direction < 0x100) {
		offset += direction;
		if (direction > 0 && offset >= 0)
			direction = 0x100;
		else if (direction < 0 &&
				offset + font->width(text) <= getSize().width())
			direction = 1;
	}
	else { // delay
		if (direction < 0x110)
			++direction;
		else
			direction = -1;
	}
	setDirty(true);
}

void Label::setText(const char *t)
{
	if (t != text) {
		if (text)
			free(text);
		text = strdup(t);
	}
	if (!font)
		font = FontFactory::getSingleton()->getDefaultFont();
	direction = 0;
	if (canScroll && font->width(text) > getSize().width()) {
		if (timer) {
			removeTimer(timer);
			delete timer;
		}
		direction = -1;
		Action<Label, int> *action =
				new Action<Label, int>(this, &Label::scroll);
		addTimer((timer = new Timer(100, action)));
	}
	else if (timer) {
		removeTimer(timer);
		delete (Action<Label, int> *)timer->getAction();
		delete timer;
		timer = NULL;
	}
	offset = 0;
	setDirty(true);
}

void Label::setAlignment(Alignment al)
{
	alignment = al;
}
