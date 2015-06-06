#ifndef _MENUFACTORY_H_
#define _MENUFACTORY_H_

#include "menu.h"
#include "list.h"

class MenuFactory
{
 public:
	MenuFactory(AbstractMenu *menu, const char *file);
	~MenuFactory();

	MenuModel *parse();

 private:
	struct line {
		int lineno;
		char *type;
		char *name;
		char *action;
		char *image;
	};

 	void append(struct line *);
	struct line *parseLine(const char *, int);
	char *parseSection(const char *, const char *);

	list<MenuModel *> menuStack;
	char path[256];
	char *filename;
	AbstractMenu *menuImplementation;
};

#endif
