#include <string.h>
#include <unistd.h>

#include "groupmenu.h"
#include "insune.h"

GroupMenu::GroupMenu(MenuModel *m, Canvas *canvas)
 : ListMenu(m, canvas), showFirstGroup(true)
{ }

GroupMenu::GroupMenu(const char *file, Canvas *canvas)
 : ListMenu(file, canvas), showFirstGroup(true)
{ }

GroupMenu::GroupMenu(MenuModel *m, const Size &s)
 : ListMenu(m, s), showFirstGroup(true)
{
}

GroupMenu::GroupMenu(const char *file, const Size &s)
 : ListMenu(file, s), showFirstGroup(true)
{ }

GroupMenu::~GroupMenu()
{
	while (modelStack.count() > 0)
		delete modelStack.pop();
}

void GroupMenu::next()
{
	const list<MenuGroup *> & groups = model->getGroupList();
	list<MenuGroup *>::iterator itg = groups.start();
	int groupPos = 0;

	for (int i = 0; itg != groups.end(); ++itg) {
		if (i + (*itg)->getItemCount() > topItem) {
			groupPos = topItem - i;
			break;
		}
		i += (*itg)->getItemCount();
	}
	if (itg == groups.end()) return;

	bool incSel = false;
	int groupCount = 0;
	showFirstGroup = true;

	for (int it = 0, i = 0; i < visibleItems || incSel; ++i) {
		if (groupPos >= (*itg)->getItemCount()) {
			++itg;
			groupPos = 0;
			groupCount++;
			if (itg == groups.end())
				break;
			if (incSel && it >= visibleItems)
				topItem++;
			continue;
		}
		if (incSel) {
			if (i >= visibleItems) {
				topItem++;
				while (i > visibleItems)
					selectedItem--, i--;
			}
			else {
				selectedItem++;
				if (selectedItem == visibleItems - 1)
					showFirstGroup = false;
			}
			break;
		}
		if (it == selectedItem)
			incSel = true;

		++groupPos, ++it;
	}
	setDirty(true);
}

void GroupMenu::draw(Canvas &canvas)
{
	const char *background[] = {
		"listmenu/item/background",
		"listmenu/item!selected/background",
		"listmenu/group/background"
	};
	const char *color[] = {
		"listmenu/item/color",
		"listmenu/item!selected/color",
		"listmenu/group/color"
	};
	const Style *style = Insune::getSingleton()->getStyle();
	Option *o;
	if ((o = style->resolve("listmenu/background")))
		canvas.drawVisual(o->getVisual(), Rect(Position(0,0), size));
	Font *font;
	if ((o = style->resolve("listmenu/font")))
		font = FontFactory::getSingleton()->loadFont(o->getValue());
	else
		font = FontFactory::getSingleton()->getDefaultFont();
	canvas.setFont(font);
	int h = font->height() * 3/2;

	const list<MenuGroup *> & groups = model->getGroupList();
	list<MenuGroup *>::iterator itg = groups.start();
	int groupPos = 0;
	int count = 0;

	for (int i = 0; itg != groups.end(); ++itg) {
		if (i + (*itg)->getItemCount() > topItem) {
			groupPos = topItem - i;
			break;
		}
		i += (*itg)->getItemCount();
		count++;
	}
	if (itg == groups.end()) return;

	bool groupStart = showFirstGroup && count != 0 && groupPos == 0;

	for (int it = 0, i = 0; i < visibleItems; ++i) {
		Rect itr = Rect(0, i * h, size.width(), (i + 1) * h);
		int y = i * h + (h - font->height()) / 2, x = 5;
		if (groupPos >= (*itg)->getItemCount()) {
			++itg;
			groupPos = 0;
			if (itg == groups.end())
				break;
			groupStart = true;
		}
		if (groupStart == true) {
			if ((o = style->resolve(background[2])))
				canvas.drawVisual(o->getVisual(), itr);
			if ((o = style->resolve(color[2])))
				canvas.setColor(atoc(o->getValue()));
			if ((*itg)->hasIcon()) {
				canvas.blit((*itg)->getIcon(),
						x, y-1, x + 16, y-1 + 16);
				x += 20;
			}
			canvas.drawText(x, y, (*itg)->getText());
			groupStart = false;
			continue;
		}
		MenuItem *item = (*itg)->getItemList().nth(groupPos);

		if ((o = style->resolve(background[(it == selectedItem)])))
			canvas.drawVisual(o->getVisual(), itr);
		if ((o = style->resolve(color[(it == selectedItem)])))
			canvas.setColor(atoc(o->getValue()));

		if (item->hasIcon()) {
			canvas.blit(item->getIcon(), x, y-1, x + 16, y-1 + 16);
			x += 20;
		}
		canvas.drawText(x, y, item->getText());
		++groupPos, ++it;
	}
}
