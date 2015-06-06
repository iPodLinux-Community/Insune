#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "insune.h"
#include "application.h"
#include "listmenu.h"

class StyleFactory
{
 public:
 	StyleFactory(const char *dir);
 	~StyleFactory();
	void setActive(Style *s);
	const list<Style *> &getStyles() const;
	
 private:
 	list<Style *> styles;
	Style *active;
};

StyleFactory::StyleFactory(const char *dirname)
 : active(0)
{
	DIR *dir;
	struct dirent *subdir;

	dir = opendir(dirname);

	while ((subdir = readdir(dir))) {
		char file[256];
		if (subdir->d_name[0] == '.')
			continue;
		snprintf(file, 256, "%s/%s/style.ss", dirname, subdir->d_name);
		if (!access(file, R_OK)) {
			Style *style = Style::parse(file);
			style->setName(subdir->d_name);
			styles.append(style);
		}
	}
	closedir(dir);
}

StyleFactory::~StyleFactory()
{
	list<Style *>::iterator it  = styles.start();
	for (; it != styles.end(); ++it)
		if ((*it) != active)
			delete (*it);
}


void StyleFactory::setActive(Style *s)
{
	active = s;
}

const list<Style *> &StyleFactory::getStyles() const
{
	return styles;
}

class StyleChooser : public Application
{
 public:
	StyleChooser();
	~StyleChooser();

	void event(Event &e);

 private:
	void selectStyle(Style *type);
	MenuModel *modelStyles();

	MenuModel *menuModel;
	StyleFactory styleFactory;
	ListMenu *menu;
};

StyleChooser::StyleChooser()
 : styleFactory(STYLEDIR)
{
	menuModel = modelStyles();
	menu = new ListMenu(menuModel, new DefaultCanvas());
	addWidget(menu);
}

StyleChooser::~StyleChooser()
{
	Insune *insune = Insune::getSingleton();
	insune->removeApplication(this);
	removeWidget(menu);
	delete menu;
	delete menuModel;
}

void StyleChooser::selectStyle(Style *s)
{
	Insune *insune = Insune::getSingleton();
	insune->setSetting("style", new Value(s->getName()));
	insune->setStyle(s);
	styleFactory.setActive(s);
}

MenuModel *StyleChooser::modelStyles()
{
	MenuModel *mm = new MenuModel();

	const list<Style *> &sl = styleFactory.getStyles();
	list<Style *>::iterator it = sl.start();
	for (; it != sl.end(); ++it) {
		Action<StyleChooser, Style *> *action;
		action = new Action<StyleChooser, Style *>();
		action->setClass(this);
		action->setMethod(&StyleChooser::selectStyle);
		action->setData(*it);
		mm->append((*it)->getName(), action);
	}
	return mm;
}

void StyleChooser::event(Event &e)
{
	switch (e.type) {
	case KEY_DOWN:
		switch (e.key.value) {
		case KEY_MENU:
			delete this;
			return;
		default:
			Application::event(e);
			break;
		}
		break;
	case BUTTON_DOWN:
		switch (e.button.button) {
		case BUTTON_MIDDLE:
			delete this;
			return;
		default:
			Application::event(e);
			break;
		}
		break;
	default:
		Application::event(e);
		break;
	}
}

APPLICATION(StyleChooser)
