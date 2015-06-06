#ifndef _LISTMENU_H_
#define _LISTMENU_H_

#include "menu.h"

class ListMenu : public AbstractMenu
{
 public:
	ListMenu(MenuModel *m, const Size &size);
	ListMenu(const char *file, const Size &size);
	ListMenu(MenuModel *m, Canvas *canvas);
	ListMenu(const char *file, Canvas *canvas);
	~ListMenu();

	void setSize(const Size &size);
	const Size &getSize() const;
	void draw(Canvas &canvas);
	void event(Event &e);
	void doSubmenu(MenuModel *);
	int getSelectedItem();

	/* control */
	virtual void next();
	virtual void prev();
	virtual void select();
	virtual void up();

 protected:
 	class ModelState {
	 public:
		ModelState(MenuModel *);
		MenuModel *model;
		int visibleItems;
		int topItem;
		int selectedItem;
	};

	void updateModel();
	void useModel();

	Size size;
	MenuModel *model;
	int visibleItems;
	int topItem;
	int selectedItem;
	list<ModelState *> modelStack;

 private:
	void init();
};

#endif
