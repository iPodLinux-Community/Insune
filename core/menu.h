#ifndef _MENU_H_
#define _MENU_H_

#include "action.h"
#include "list.h"
#include "widget.h"
#include "image.h"
#include "smartpointer.h"

class MenuItem
{
 public:
 	MenuItem(const char *str, Image *ico, const ActionBase *act);
	~MenuItem();

	void activate() const;
	const char *getText() const;
	Image &getIcon();
	bool hasIcon();

 private:
 	char *string;
	SmartPointer<Image> smartIcon;
	const ActionBase *action;
};

class MenuGroup
{
 public:
 	MenuGroup(const char *str, Image *ico);
	~MenuGroup();

	const char *getText() const;
	Image &getIcon();
	bool hasIcon();
	void append(MenuItem *item);
	void insert(MenuItem *item, int at);
	int getItemCount();
	const list<MenuItem *> &getItemList() const;

 private:
	char *string;
	SmartPointer<Image> smartIcon;
	int itemCount;
 	list<MenuItem *> items;
};

class MenuModel
{
 public:
	MenuModel();
	virtual ~MenuModel();

	void append(const char *str, const ActionBase *action);
	void insert(const char *str, const ActionBase *act, int at);
	void append(const char *str, Image* ico, const ActionBase *action);
	void insert(const char *str, Image* ico, const ActionBase *act, int at);
	void appendGroup(const char *str);
	void appendGroup(const char *str, Image *ico);
	void insertGroup(const char *str, int at);
	void insertGroup(const char *str, Image *ico, int at);
	void endGroup();
	int getItemCount();
	int getGroupCount();
	const list<MenuItem *> &getItemList() const;
	const list<MenuGroup *> &getGroupList() const;

 private:
 	int itemCount;
	int groupCount;
	list<MenuItem *> items;
	list<MenuGroup *> groups;
	MenuGroup *group;
};

class AbstractMenu : public Widget
{
 protected:
	AbstractMenu(Canvas *c = NULL);
 public:
	void doExec(const char *);
	virtual void doSubmenu(MenuModel *) = 0;
	void doModule(const char *);
};

#endif
