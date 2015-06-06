#include <string.h>
#include <unistd.h>

#include "menu.h"
#include "menufactory.h"
#include "insune.h"

MenuItem::MenuItem(const char *str, Image *ico, const ActionBase *act)
 : smartIcon(ico), action(act)
{
	string = strdup(str);
}

MenuItem::~MenuItem()
{
	free(string);
	delete action;
}

void MenuItem::activate() const
{
	action->callback();
}

const char *MenuItem::getText() const
{
	return string;
}

Image &MenuItem::getIcon()
{
	return *smartIcon;
}

bool MenuItem::hasIcon()
{
	return (smartIcon != NULL);
}


MenuGroup::MenuGroup(const char *str, Image *ico)
 : smartIcon(ico), itemCount(0)
{
	string = strdup(str);
}

MenuGroup::~MenuGroup()
{
	free(string);
}

const char *MenuGroup::getText() const
{
	return string;
}

Image &MenuGroup::getIcon()
{
	return *smartIcon;
}

bool MenuGroup::hasIcon()
{
	return (smartIcon != NULL);
}

void MenuGroup::append(MenuItem *item)
{
	++itemCount;
	items.append(item);
}

void MenuGroup::insert(MenuItem *item, int at)
{
	++itemCount;
	items.insert(item, items.nth(at));
}

int MenuGroup::getItemCount()
{
	return itemCount;
}

const list<MenuItem *> &MenuGroup::getItemList() const
{
	return items;
}


MenuModel::MenuModel()
 : itemCount(0), groupCount(0)
{
	appendGroup("Ungrouped");
}

MenuModel::~MenuModel()
{
	list<MenuItem *>::iterator iti = items.start();
	for (; iti != items.end(); ++iti)
		delete (*iti);

	list<MenuGroup *>::iterator itg = groups.start();
	for (; itg != groups.end(); ++itg)
		delete (*itg);
}

void MenuModel::append(const char *str, const ActionBase *act)
{
	append(str, NULL, act);
}

void MenuModel::insert(const char *str, const ActionBase *act, int at)
{
	insert(str, NULL, act, at);
}

void MenuModel::append(const char *str, Image *ico, const ActionBase *act)
{
	MenuItem *item = new MenuItem(str, ico, act);

	++itemCount;
	items.append(item);

	group->append(item);
}

void MenuModel::insert(const char *str, Image *ico, const ActionBase *act, int at)
{
	MenuItem *item = new MenuItem(str, ico, act);

	++itemCount;
	items.insert(item, items.nth(at));

	int c, p;
	list<MenuGroup *>::iterator it = groups.start();
	for (c = 0, p = 0; it != groups.end(); ++it){
		if (at >= p && at < p + (*it)->getItemCount()){
			(*it)->insert(item, at - p);
		}
		p += (*it)->getItemCount();
		c++;
	}
}

void MenuModel::appendGroup(const char *str)
{
	appendGroup(str, NULL);
}

void MenuModel::appendGroup(const char *str, Image *ico)
{
	group = new MenuGroup(str, ico);

	++groupCount;
	groups.append(group);
}

void MenuModel::insertGroup(const char *str, int at)
{
	insertGroup(str, NULL, at);
}

void MenuModel::insertGroup(const char *str, Image *ico, int at)
{
	group = new MenuGroup(str, ico);

	++groupCount;
	groups.insert(group, groups.nth(at));
}

void MenuModel::endGroup()
{
	group = groups.first();
}

int MenuModel::getItemCount()
{
	return itemCount;
}

int MenuModel::getGroupCount()
{
	return groupCount;
}

const list<MenuItem *> &MenuModel::getItemList() const
{
	return items;
}

const list<MenuGroup *> &MenuModel::getGroupList() const
{
	return groups;
}

AbstractMenu::AbstractMenu(Canvas *c)
 : Widget(c)
{ }

void AbstractMenu::doExec(const char *cmd)
{
	warn("exec %s\n", cmd);
}

void AbstractMenu::doModule(const char *module)
{
	Insune *insune = Insune::getSingleton();
	ApplicationFactory *appf = ApplicationFactory::getSingleton();
	Application *app = appf->openApplication(module);
	if (app)
		insune->addApplication(app);
}
