#ifndef _GROUPMENU_H_
#define _GROUPMENU_H_

#include "listmenu.h"

class GroupMenu : public ListMenu
{
 public:
	GroupMenu(MenuModel *m, Canvas *canvas);
	GroupMenu(const char *file, Canvas *canvas);
	GroupMenu(MenuModel *m, const Size &size);
	GroupMenu(const char *file, const Size &size);
	~GroupMenu();

	void draw(Canvas &canvas);
 protected:
	void next();
	bool showFirstGroup;
};

#endif
