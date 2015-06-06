/*
 * Copyright (c) 2008 Courtney Cavin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menufactory.h"
#include "insune.h"

MenuFactory::MenuFactory(AbstractMenu *menu, const char *file)
 : menuImplementation(menu)
{
	filename = strdup(file);
}

MenuFactory::~MenuFactory()
{
	free(filename);
}

char *MenuFactory::parseSection(const char *line, const char *sep)
{
	const char *s;
	char *p, *ret, *c;

	c = strchr(line, '#');
	s = line;
	if ((p = strchr(line, sep[0])) == NULL)
		return NULL;
	while (p != s && *(p - 1) == '\\') {
		s = p++;
		if ((p = strchr(p, sep[0])) == NULL)
			return NULL;
	}

	s = ++p;
	do {
		if ((p = strchr(p, sep[1])) == NULL)
			return NULL;
	} while (*(p - 1) == '\\');

	if (p - s < 1 || (c != NULL && (s > c || p > c)))
		return NULL;
	ret = (char *)xmalloc(sizeof(char) * ((p - s) + 1));
	strncpy(ret, s, (p - s));
	ret[p - s] = '\0';
	return ret;
}

MenuFactory::line *MenuFactory::parseLine(const char *line, int lineno)
{
	char asep[] = "[]";
	char bsep[] = "()"; 
	char csep[] = "{}"; 
	char dsep[] = "<>"; 

	MenuFactory::line *ret = (MenuFactory::line *)xmalloc(sizeof(MenuFactory::line));
	ret->lineno = lineno;
	ret->type = parseSection(line, asep);
	ret->name = parseSection(line, bsep);
	ret->action = parseSection(line, csep);
	ret->image = parseSection(line, dsep);
	return ret;
}

void MenuFactory::append(MenuFactory::line *line)
{
	if (line->type == NULL) {
		warn("No item type specified ");
		goto fail;
	}

	if (strcasecmp(line->type, "exec") == 0) {
		Action<AbstractMenu, const char *> *action;
		action = new Action<AbstractMenu, const char *>();
		if (line->action == NULL) {
			warn("No action specified for exec ");
			goto fail;
		}
		if (line->name == NULL) {
			warn("No item name specified ");
			goto fail;
		}
		action->setClass(menuImplementation);
		action->setMethod(&AbstractMenu::doExec);
		action->setData(line->action);
		snprintf(path, 256, DATADIR "%s", line->image);

		try {
			Image *image = Image::load(path);
			menuStack.peek()->append(line->name, image, action);
		} catch (Exception &e) {
			menuStack.peek()->append(line->name, action);
			warn(e.getMessage());
			warn(" at %s:%d\n", filename, line->lineno);
		}
	}
	else if (strcasecmp(line->type, "submenu") == 0) {
		Action<AbstractMenu, MenuModel *> *action;
		action = new Action<AbstractMenu, MenuModel *>();

		if (line->name == NULL) {
			warn("No item name specified ");
			goto fail;
		}
		MenuModel *mm = new MenuModel();
		action->setClass(menuImplementation);
		action->setMethod(&AbstractMenu::doSubmenu);
		action->setData(mm);
		safe_free(line->action);
		snprintf(path, 256, DATADIR "%s", line->image);

		try {
			Image *image = Image::load(path);
			menuStack.peek()->append(line->name, image, action);
			menuStack.push(mm);
		} catch (Exception &e) {
			menuStack.peek()->append(line->name, action);
			menuStack.push(mm);
			warn(e.getMessage());
			warn(" at %s:%d\n", filename, line->lineno);
		}
	}
	else if (strcasecmp(line->type, "module") == 0) {
		Action<AbstractMenu, const char *> *action;
		action = new Action<AbstractMenu, const char *>();
		if (line->action == NULL) {
			warn("No module name specified ");
			goto fail;
		}
		if (line->name == NULL) {
			warn("No item name specified ");
			goto fail;
		}
		action->setClass(menuImplementation);
		action->setMethod(&AbstractMenu::doModule);
		action->setData(line->action);
		snprintf(path, 256, DATADIR "%s", line->image);

		try {
			Image *image = Image::load(path);
			menuStack.peek()->append(line->name, image, action);
		} catch (Exception &e) {
			menuStack.peek()->append(line->name, action);
			warn(e.getMessage());
			warn(" at %s:%d\n", filename, line->lineno);
		}

	}
	else if (strcasecmp(line->type, "group") == 0) {
		if (line->name == NULL) {
			warn("No group name specified ");
			goto fail;
		}
		snprintf(path, 256, DATADIR "%s", line->image);

		try {

			Image *image = Image::load(path);
			menuStack.peek()->appendGroup(line->name, image);
		} catch (Exception &e) {
			menuStack.peek()->appendGroup(line->name);
			warn(e.getMessage());
			warn(" at %s:%d\n", filename, line->lineno);
		}
	}
	else if (strcasecmp(line->type, "endgroup") == 0) {
		menuStack.peek()->endGroup();
	}
	else if (strcasecmp(line->type, "end") == 0) {
		safe_free(line->type);
		safe_free(line->name);
		safe_free(line->action);
		safe_free(line->image);
		safe_free(line);
		menuStack.pop();		
		return;
	}
	else {
		warn("Invalid type `%s' ", line->type);
		goto fail;
	}

	safe_free(line->name);
	safe_free(line->type);
	safe_free(line->image);
	safe_free(line);

	return;
fail:
	warn("at %s:%d\n", filename, line->lineno);
	safe_free(line->type);
	safe_free(line->name);
	safe_free(line->action);
	safe_free(line->image);
	free(line);
}

MenuModel *MenuFactory::parse()
{
	FILE *fp;
	char line[1024];
	int lineno = 0;

	if ((fp = fopen(filename, "r")) == NULL) {
		warn("No such menu: %s\n", filename);
		return new MenuModel();
	}

	menuStack.push(new MenuModel());
	while (fgets(line, 1024, fp))
		append(parseLine(line, ++lineno));

	fclose(fp);
	return menuStack.pop();
}
