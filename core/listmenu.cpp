#include <string.h>
#include <unistd.h>

#include "listmenu.h"
#include "menufactory.h"
#include "insune.h"

ListMenu::ModelState::ModelState(MenuModel *m)
 : model(m), visibleItems(0), topItem(0), selectedItem(0)
{ }

ListMenu::ListMenu(MenuModel *m, Canvas *canvas)
 : AbstractMenu(canvas), size(Size(canvas->width(), canvas->height()))
{
	modelStack.push(new ModelState(m));
	init();
}

ListMenu::ListMenu(const char *file, Canvas *canvas)
 : AbstractMenu(canvas), size(Size(canvas->width(), canvas->height()))
{
	MenuFactory mf = MenuFactory(this, file);
	modelStack.push(new ModelState(mf.parse()));
	init();
}

ListMenu::ListMenu(MenuModel *m, const Size &s)
 : size(s)
{
	modelStack.push(new ModelState(m));
	init();
}

ListMenu::ListMenu(const char *file, const Size &s)
 : size(s)
{
	MenuFactory mf = MenuFactory(this, file);
	modelStack.push(new ModelState(mf.parse()));
	init();
}

ListMenu::~ListMenu()
{
	if (modelStack.count() > 0)
		delete modelStack.pop();
}

void ListMenu::init()
{
	useModel();
	setSize(size);
}

int ListMenu::getSelectedItem()
{
	return topItem + selectedItem;
}

void ListMenu::setSize(const Size &s)
{
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;
	Font *font;
	if ((o = style->resolve("listmenu/font")))
		font = FontFactory::getSingleton()->loadFont(o->getValue());
	else
		font = FontFactory::getSingleton()->getDefaultFont();
	size = s;
	visibleItems = s.height() / (font->height()*3/2);
}

const Size &ListMenu::getSize() const
{
	return size;
}

void ListMenu::useModel()
{
	model = modelStack.peek()->model;
	visibleItems = modelStack.peek()->visibleItems;
	topItem = modelStack.peek()->topItem;
	selectedItem = modelStack.peek()->selectedItem;
}

void ListMenu::updateModel()
{
	modelStack.peek()->model = model;
	modelStack.peek()->visibleItems = visibleItems;
	modelStack.peek()->topItem = topItem;
	modelStack.peek()->selectedItem = selectedItem;
}

void ListMenu::draw(Canvas &canvas)
{
	const char *background[] = {
		"listmenu/item/background",
		"listmenu/item!selected/background"
	};
	const char *color[] = {
		"listmenu/item/color",
		"listmenu/item!selected/color"
	};
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;
	if ((o = style->resolve("listmenu/background")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0), size));
	int count = visibleItems;
	int total = model->getItemCount() - topItem;
	if (count > total)
		count = total;
	Font *font;
	if ((o = style->resolve("listmenu/font")))
		font = FontFactory::getSingleton()->loadFont(o->getValue());
	else
		font = FontFactory::getSingleton()->getDefaultFont();
	canvas.setFont(font);

	for (int i = 0; i < count; ++i) {
		int h = font->height() * 3/2, y, x = 5;
		Rect itr = Rect(0, i * h, size.width(), (i + 1) * h);
		if ((o = style->resolve(background[(i == selectedItem)])))
			canvas.drawVisual(o->getVisual(), itr);
		if ((o = style->resolve(color[(i == selectedItem)])))
			canvas.setColor(atoc(o->getValue()));

		MenuItem *item = model->getItemList().nth(i + topItem);
		if (item->hasIcon()){
			y = i*h + (h - font->height()) / 2 - 1;
			canvas.blit(item->getIcon(), x, y, x + 16, y + 16);
			x += 20;
		}
		canvas.drawText(x, i*h + (h - font->height()) / 2,
			item->getText());
	}
}

void ListMenu::prev()
{
	if (model->getItemCount() == 0)
		return;
	if (selectedItem == 0) {
		if (topItem != 0)
			topItem--;
	}
	else
		selectedItem--;
	setDirty(true);
}

void ListMenu::next()
{
	int itemCount = model->getItemCount();
	if (itemCount == 0)
		return;

	if (selectedItem < visibleItems - 1) {
		if (selectedItem < itemCount - topItem -1)
			selectedItem++;
	}
	else if (topItem < itemCount - visibleItems)
		topItem++;
	setDirty(true);
}

void ListMenu::select()
{
	int itemCount = model->getItemCount();

	if (topItem + selectedItem > itemCount || itemCount == 0)
		return;
	model->getItemList().nth(topItem + selectedItem)->activate();
}

void ListMenu::up()
{
	if (modelStack.count() > 1) {
		delete modelStack.pop();
		useModel();
	}
	setDirty(true);
}

void ListMenu::event(Event &e)
{
	switch (e.type) {
	case KEY_DOWN:
		switch (e.key.value) {
		case KEY_ACTION:
			select();
			break;
		case SCROLL_LEFT:
			SCROLL_MOD(5);
			prev();
			break;
		case SCROLL_RIGHT:
			SCROLL_MOD(5);
			next();
			break;
		case KEY_MENU:
			up();
			break;
		default:break;
		}
		break;
	case BUTTON_DOWN:
		switch (e.button.button) {
		case BUTTON_LEFT:
			select();
			break;
		case BUTTON_MIDDLE:
			up();
			break;
		case BUTTON_WHEELUP:
			prev();
			break;
		case BUTTON_WHEELDOWN:
			next();
			break;
		default:break;
		}
		break;
	default:break;
	}
}

void ListMenu::doSubmenu(MenuModel *mm)
{
	updateModel();
	modelStack.push(new ModelState(mm));
	useModel();
	setSize(size);
	setDirty(true);
}
