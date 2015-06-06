#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "insune.h"
#include "application.h"
#include "menu.h"
#include "groupmenu.h"

class Filebrowser : public Application
{
 public:
	Filebrowser();
	~Filebrowser();

	MenuModel *modelDirectory(const char *dirname);

	void event(Event &e);

	struct Entry {
		char *name;
		mode_t mode;
		bool operator<(const Entry &a) const
		{
			if (S_ISDIR(mode) != S_ISDIR(a.mode))
				return S_ISDIR(mode) ? true : false;
			return (strcasecmp(name, a.name) < 0);
		}
		bool operator>(const Entry &a) const
		{
			if (S_ISDIR(mode) != S_ISDIR(a.mode))
				return S_ISDIR(mode) ? false : true;
			return (strcasecmp(name, a.name) > 0);
		}
		bool operator==(const Entry &a) const
		{
			if (S_ISDIR(mode) != S_ISDIR(a.mode))
				return false;
			return (strcasecmp(name, a.name) == 0);
		}
	};

 private:
 	int start;
	void previousDirectory(int);
	void handleEntry(Entry *);
	GroupMenu *menu;
	list<int> directoryStack;
	list<MenuModel *> modelStack;
};

Filebrowser::Filebrowser()
{
	start = open(".", O_RDONLY);
	menu = new GroupMenu(modelDirectory("."), new DefaultCanvas());
	addWidget(menu);
}

Filebrowser::~Filebrowser()
{
	Insune *insune = Insune::getSingleton();
	insune->removeApplication(this);
	removeWidget(menu);
	fchdir(start);
	close(start);
	while (directoryStack.count() > 0)
		close(directoryStack.pop());
	while (modelStack.count() > 0)
		delete modelStack.pop();
	delete menu;
}

void Filebrowser::previousDirectory(int)
{
	if (directoryStack.count() == 0) {
		chdir("..");
		menu->doSubmenu(modelDirectory("."));
	} else {
		int fd = directoryStack.pop();
		fchdir(fd);
		close(fd);
		menu->up();
		delete modelStack.pop();
	}
}

void Filebrowser::handleEntry(Entry *e)
{
	if (S_ISDIR(e->mode)) {
		directoryStack.push(open(".", O_RDONLY));
		chdir(e->name);
		menu->doSubmenu(modelDirectory("."));
	} else {
	}
}

MenuModel *Filebrowser::modelDirectory(const char *dirname)
{
	list<Entry *> entryList;
	MenuModel *mm = new MenuModel();
	DIR *dir;
	struct stat st;
	struct dirent *subdir;

	dir = opendir(dirname);

	mm->appendGroup("Directories");
	while ((subdir = readdir(dir))) {
		if (strcmp(subdir->d_name, ".") == 0)
			continue;
		stat(subdir->d_name, &st);

		if (strcmp(subdir->d_name, "..") == 0) {
			Action<Filebrowser, int> *action;
			action = new Action<Filebrowser, int>();
			action->setClass(this);
			action->setMethod(&Filebrowser::previousDirectory);
			mm->append(".. [Parent Directory]", action);
		} else {
			Entry *entry = new Entry;
			entry->name = strdup(subdir->d_name);
			entry->mode = st.st_mode;
			entryList.append(entry);
		}
	}
	closedir(dir);
	
	entryList.sort();

	bool files = false;
	list<Entry *>::iterator it = entryList.start();
	for (; it != entryList.end(); ++it) {
		Entry *entry = (*it);
		if (!files && !S_ISDIR(entry->mode)) {
			files = true;
			mm->endGroup();
			mm->appendGroup("Files");
		}
		Action<Filebrowser, Entry *> *action;
		action = new Action<Filebrowser, Entry *>();
		action->setClass(this);
		action->setMethod(&Filebrowser::handleEntry);
		action->setData(entry);
		mm->append(entry->name, action);
	}
	mm->endGroup();
	modelStack.push(mm);
	return mm;
}

void Filebrowser::event(Event &e)
{
	switch (e.type) {
	case KEY_DOWN:
		switch (e.key.value) {
		case KEY_MENU:
			unload();
			return;
		default:
			Application::event(e);
			break;
		}
		break;
	case BUTTON_DOWN:
		switch (e.button.button) {
		case BUTTON_MIDDLE:
			unload();
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

APPLICATION(Filebrowser)
