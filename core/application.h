#ifndef _APPLICATION_H_
#define _APPLICATION_H_
#include "list.h"

#include "widget.h"
#include "setting.h"

class Application : public WidgetContainer
{
 public:
	Application();
	virtual ~Application();

	virtual void update();
	virtual void draw();
	virtual void event(Event &e);

	bool done() const;

 protected:
	const char *getDataPath() const;
	void saveSettings();

	Setting *getSetting(const char *key) const;
	void addSetting(Setting *s);

	void addWidget(Widget *widget);
	void removeWidget(Widget *widget);
	void activate(Widget *w);
	void unload();

 private:
	void loadSettings(const char *filename);

	Widget *activeWidget;
	SettingFile settings;
	char *datapath;
	bool shouldUnload;
};

class ApplicationFactory
{
 public:
	static ApplicationFactory *getSingleton();

	Application *openApplication(const char *name);
	void closeApplication(Application *app);

	char *getDataPath(Application *) const;
	char *getSettingsPath(Application *) const;

 private:
 	
	class AppWrap
	{
	 public:
		AppWrap(const char *name, void *handle);
		~AppWrap();
		void setLink(Application *app);
		Application *getLink() const;
		const char *getName() const;

	 private:
		Application *link;
		void *handle;
		char *name;
	};
	AppWrap *findWrapper(Application *) const;

	ApplicationFactory();
	~ApplicationFactory();

	list<AppWrap *> apps;
	AppWrap *currentApp;
	static ApplicationFactory *singleton;
};

#define APPLICATION(x) \
extern "C" Application *__appinit() { return new x(); }

#define INSTANCE(x) \
extern "C" Application *__appinit() { x(); return 0; }

#endif
