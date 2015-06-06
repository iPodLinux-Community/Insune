#ifndef _INSUNE_H_
#define _INSUNE_H_

#include "application.h"
#include "setting.h"
#include "style.h"
#include "dialog.h"
#include "transition.h"
#include "config.h"

class Insune {
 public:
	static Insune *getSingleton();

	void addApplication(Application *app);
	void removeApplication(Application *app);

	void event();
	void update();
	void draw();

	int width() const;
	int height() const;
	void addCanvas(Canvas *canvas);
	void addCanvas(Canvas *canvas, Transition *transition);
	void removeCanvas(Canvas *canvas);

	void showDialog(Dialog *);
	void closeDialog(Dialog *);
	
	void addSetting(Setting *s);
	Setting *getSetting(const char *str);
	void setSetting(const char *str, Value *value);

	int getVersion();

	const Style *getStyle() const;
	void setStyle(Style *style);

	void preload();

	void exit(int code = 0);
	void dispose();

	Engine &getEngine();

 private:
	~Insune();
	Insune();

	bool implicitRedraw;
	list<Application *> apps;
	SettingFile settings;
	Engine engine;
	Application *active;
	Style *style;
	list<Transition *> transitions;
	static Insune *singleton;
};

void warn(const char *fmt, ...);

#endif
