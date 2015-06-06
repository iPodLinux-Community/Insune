#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

#include "insune.h"
#include "application.h"
#include "setting.h"

ApplicationFactory::AppWrap::AppWrap(const char *name, void *handle)
 : link(0)
{
	this->name = strdup(name);
	this->handle = handle;
}

ApplicationFactory::AppWrap::~AppWrap()
{
	free(name);
	dlclose(handle);
}

void ApplicationFactory::AppWrap::setLink(Application *l)
{
	link = l;
}

Application *ApplicationFactory::AppWrap::getLink() const
{
	return link;
}

const char *ApplicationFactory::AppWrap::getName() const
{
	return name;
}

ApplicationFactory *ApplicationFactory::singleton = 0;

ApplicationFactory *ApplicationFactory::getSingleton()
{
	if (!singleton)
		singleton = new ApplicationFactory();
	return singleton;
}

Application *ApplicationFactory::openApplication(const char *name)
{
	void *handle;
	Application *(* init)(void);
	char *error;
	char filename[256];

	snprintf(filename, 256, APPDIR "%s/application.so", name);
	handle = dlopen(filename, RTLD_LAZY);

	if (!handle) {
		warn("%s\n", dlerror());
		return NULL;
	}

	dlerror();

	*(void **)(&init) = dlsym(handle, "__appinit");
	if ((error = dlerror()) != NULL) {
		warn("%s\n", error);
		dlclose(handle);
		return NULL;
	}

	AppWrap *app = new AppWrap(name, handle);
	currentApp = app;

	app->setLink((* init)()); 
	currentApp = NULL;

	if (app->getLink()) {
		apps.append(app);
		return app->getLink();
	}
	else
		delete app;
	return NULL;
}

void ApplicationFactory::closeApplication(Application *app)
{
	AppWrap *aw = findWrapper(app);
	if (aw) {
		apps.remove(aw);
		delete aw->getLink();
		delete aw;
	}
}

ApplicationFactory::AppWrap *ApplicationFactory::findWrapper(Application *app) const
{
	list<AppWrap *>::iterator it = apps.start();
	for (; it != apps.end(); ++it) {
		AppWrap *caw = (*it);
		if (caw->getLink() == app)
			return caw;
	}
	/* if an application is constructing, we can assume that
	 * it is what is wanted */
	if (currentApp)
		return currentApp;
	return NULL;
}


char *ApplicationFactory::getDataPath(Application *app) const
{
	AppWrap *aw = findWrapper(app);
	if (aw) {
		char path[256];
		snprintf(path, 256, APPDIR "%s/data", aw->getName());
		return strdup(path);
	}
	return NULL;
}

char *ApplicationFactory::getSettingsPath(Application *app) const
{
	AppWrap *aw = findWrapper(app);
	if (aw) {
		char path[256];
		snprintf(path, 256, SETTINGDIR "%s.settings", aw->getName());
		return strdup(path);
	}
	return NULL;
}


ApplicationFactory::ApplicationFactory()
 : currentApp(0)
{ }

ApplicationFactory::~ApplicationFactory()
{
	list<AppWrap *>::iterator it = apps.start();
	for (; it != apps.end(); ++it) {
		AppWrap *aw = (*it);
		delete aw;
	}
	apps.destroy();
}


Application::Application()
 : activeWidget(NULL), shouldUnload(false)
{
	char *settingspath;
	ApplicationFactory *af = ApplicationFactory::getSingleton();
	datapath = af->getDataPath(this);
	if ((settingspath = af->getSettingsPath(this))) {
		if (!access(settingspath, R_OK))
			loadSettings(settingspath);
		free(settingspath);
	}
}

Application::~Application()
{
	if (datapath)
		free(datapath);
	saveSettings();
}

static inline void do_addWidget(Widget *w)
{
	Insune *insune = Insune::getSingleton();
	if (w->getCanvas()) {
		if (w->hasTransition())
			insune->addCanvas(w->getCanvas());
		else
			insune->addCanvas(w->getCanvas(), NULL);
	}
	const list<Widget *> &widgets = w->getChildren();
	list<Widget *>::iterator it = widgets.start();
	for (; it != widgets.end(); ++it)
		do_addWidget((*it));
}

void Application::addWidget(Widget *w)
{
	if (!activeWidget)
		activeWidget = w;
	WidgetContainer::addWidget(w);

	do_addWidget(w);
}

static inline void do_removeWidget(Widget *w)
{
	Insune *insune = Insune::getSingleton();
	const list<Widget *> &widgets = w->getChildren();
	list<Widget *>::iterator it = widgets.start();
	for (; it != widgets.end(); ++it)
		do_removeWidget((*it));
	if (w->getCanvas())
		insune->removeCanvas(w->getCanvas());
}

void Application::removeWidget(Widget *w)
{
	do_removeWidget(w);
	WidgetContainer::removeWidget(w);
}

void Application::activate(Widget *w)
{
	activeWidget = w;
}

const char *Application::getDataPath() const
{
	return datapath;
}

void Application::addSetting(Setting *s)
{
	settings.addSetting(s);
}

Setting *Application::getSetting(const char *tag) const
{
	return settings.getSetting(tag);
}

void Application::loadSettings(const char *filename)
{
	settings.load(filename);
}

void Application::saveSettings()
{
	settings.save();
}

void Application::draw()
{
	drawChildren();
}

void Application::update()
{
	updateChildren();
}

void Application::event(Event &e)
{
	if (activeWidget)
		activeWidget->event(e);
}

void Application::unload()
{
	shouldUnload = true;
}

bool Application::done() const
{
	return shouldUnload;
}
