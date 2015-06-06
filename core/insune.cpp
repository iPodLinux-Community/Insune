#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <dlfcn.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "engine.h"
#include "canvas.h"
#include "action.h"
#include "list.h"
#include "insune.h"
#include "hardware.h"

#include "menu.h"
#include "groupmenu.h"
#include "menufactory.h"
#include "exception.h"

#ifdef UNITTESTS
extern bool unitTest();
#endif

unsigned int get_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned int)((tv.tv_sec % 0x3fffff)*1000 + tv.tv_usec/1000);
}

void warn(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);
	fflush(stderr);
}

#if 0
#define HEADER_LEFT   2
#define HEADER_RIGHT  3
#define HEADER_CENTER 1

class HeaderText : public Widget
{
 public:
	HeaderText()
	 : size(getDefaultFont()->width("insune"), 20)
	{ }

	const Size &getSize()
	{
		return size;
	}

	void setSize(const Size &size)
	{
		this->size = size;
	}

	void draw(Canvas &canvas)
	{
		const Style *style = Insune::getSingleton()->getStyle();
		Option *o;
		if ((o = style->resolve("header/color")))
			canvas.setColor(atoc(o->getValue()));
		else
			canvas.setColor(0xff000000);
		canvas.drawText(0, 0, "insune");
	}

	void event(Event &) { }

 private:
	Rect bounds;
};

class ClockHeaderWidget : public Widget
{
 public:
	ClockHeaderWidget()
	 : size(19, 19)
	{ }

	const Rect &getWindow()
	{
		return bounds;
	}

	void setWindow(const Rect &rect)
	{
		bounds = rect;
	}

	void draw(Canvas &canvas)
	{
		time_t tis;
		struct tm ti;
		time(&tis);
		localtime_r(&tis, &ti);
		int r = (bounds.height() + 1) / 2 - 2;
		int cx = bounds.left() + r+1, cy = bounds.top() + r+1;
		const Style *style = Insune::getSingleton()->getStyle();
		Option *o;
		if ((o = style->resolve("header/clock/color")))
			canvas.setColor(atoc(o->getValue()));
		else
			canvas.setColor(0xff000000);
		if ((o = style->resolve("header/clock/background")))
			canvas.drawVisual(o->getVisual(), bounds);
		else {
			canvas.setAA(false);
			canvas.drawCircle(cx, cy, r);
			canvas.drawCircle(cx, cy, r-1);
		}
		canvas.setAA(true);
		int hx = (int)(r/2 * cos((ti.tm_hour%12)*M_PI/6 - M_PI_2)+0.5),
			hy = (int)(r/2 * sin((ti.tm_hour%12)*M_PI/6 - M_PI_2)+0.5);
		canvas.drawLine(cx, cy, cx + hx, cy + hy);
		int mx = (int)(r * cos(ti.tm_min * M_PI / 30 - M_PI_2)),
			my = (int)(r * sin(ti.tm_min * M_PI /30 - M_PI_2));
		canvas.drawLine(cx, cy, cx + mx, cy + my);
	}

	void event(Event &) { }

 private:
	Rect bounds;
};

class Header : public Widget
{
	struct HeaderWidget {
		Widget *widget;
		int position;
	};
 public:
	Header(const Size &s)
	 : size(s)
	{
		setSize(s);
	}

	const Size &getSize()
	{
		return size;
	}

	void setSize(const Size &size)
	{
		this->size = size;
		revalidate();
	}

	void draw(Canvas &canvas)
	{
		const Style *style = Insune::getSingleton()->getStyle();
		Option *o;
		if ((o = style->resolve("header/background")))
			canvas.drawVisual(o->getVisual(), bounds);
		const Position &pos = getPosition();

		list<HeaderWidget *>::iterator it = widgets.start();
		for (; it != widgets.end(); ++it) {
			Widget *w = (*it)->widget;
			w->draw(canvas);
		}
	}

	void event(Event &e) { }

	void addWidget(Widget *widget, int position)
	{
		HeaderWidget *w = new HeaderWidget;
		w->widget = widget;
		w->position = position;
		widgets.append(w);
		revalidate();
	}

	void removeWidget(Widget *widget)
	{
		list<HeaderWidget *>::iterator it = widgets.start();
		for (; it != widgets.end(); ++it) {
			HeaderWidget *w = (*it);
			if (w->widget == widget) {
				widgets.remove(w);
				delete w;
				break;
			}
		}
		revalidate();
	}

 private:
 	void revalidate()
	{
		int rl = bounds.right();
		int lr = bounds.left();
		int pad = rl/40;

		const Position &pos = getPosition();
		list<HeaderWidget *>::iterator it = widgets.start();
		for (; it != widgets.end(); ++it) {
			HeaderWidget *hw = (*it);
			Widget *w = hw->widget;
			const Size &win = w->getSize();
			switch (hw->position) {
			case HEADER_LEFT:
				w->setPosition(Position(lr + pad, pos.Y()));
				lr += pad + win.width();
				break;
			case HEADER_RIGHT:
				rl -= pad + win.width();
				w->setPosition(Position(rl, pos.Y()));
				break;
			case HEADER_CENTER:
				w->setPosition(Position(
					(size.width() - win.width()) / 2,
					pos.Y()));
				break;
			}
		}
	}

	Size size;
	list<HeaderWidget *> widgets;
};
#endif

class MainApplication : public Application
{
 public:
	MainApplication()
	{
		#if 0
		Header *h = new Header(Size(canvas->width(), 20));
		h->addWidget(new ClockHeaderWidget(), HEADER_LEFT);
		h->addWidget(new HeaderText(), HEADER_CENTER);
		addWidget(h);
		#endif

		addWidget(new GroupMenu(DATADIR "menu", new DefaultCanvas()));
	}

 private:
};

Insune *Insune::singleton = 0;

Insune *Insune::getSingleton()
{
	if (!singleton)
		singleton = new Insune();
	return singleton;
}

void Insune::addApplication(Application *app)
{
	apps.push(app);
	active = app;
}

void Insune::removeApplication(Application *app)
{
	apps.remove(app);
	if (active == app)
		active = apps.peek();
}

void Insune::event()
{
	Event ev;
	while (engine.pollEvent(&ev))
		active->event(ev);
}

void Insune::update()
{
	list<Application *>::iterator it = apps.start();
	while (it != apps.end()) {
		Application *app = (*it);
		++it;
		if (app->done()) {
			removeApplication(app);
			ApplicationFactory::getSingleton()->closeApplication(app);
		}
		else 
			app->update();
	}
	list<Transition *>::iterator itt = transitions.start();
	while (itt != transitions.end()) {
		Transition *t = (*itt); ++itt;
		t->step();
		if (t->isFinished()) {
			transitions.remove(t);
			delete t;
		}
	}
	//engine.animate();
}

void Insune::draw()
{
	//if (!implicitRedraw) return;
	list<Application *>::iterator it = apps.start();
	for (; it != apps.end(); ++it)
		(*it)->draw();
	engine.render();
	implicitRedraw = false;
}

void Insune::addCanvas(Canvas *canvas, Transition *transition)
{
	
	if (canvas) {
		engine.add(canvas->object());
		if (transition)
			transitions.append(transition);
	}
	implicitRedraw = true;
}

void Insune::addCanvas(Canvas *canvas)
{
	addCanvas(canvas, new FadeTransition(canvas, TRANSITION_IN));
}

Engine &Insune::getEngine()
{
	return engine;
}

void Insune::removeCanvas(Canvas *canvas)
{
	if (canvas) {
		list<Transition *>::iterator it = transitions.start();
		while (it != transitions.end()) {
			Transition *t = (*it); ++it;
			t->step();
			if (t->getCanvas() == canvas) {
				transitions.remove(t);
				delete t;
			}
		}
		//transitions.append(new FadeTransition(canvas, TRANSITION_OUT);
		engine.remove(canvas->object());
	}
	implicitRedraw = true;
}

int Insune::width() const
{
	return engine.width();
}

int Insune::height() const
{
	return engine.height();
}

void Insune::addSetting(Setting *s)
{
	settings.addSetting(s);
}

Setting *Insune::getSetting(const char *str)
{
	return settings.getSetting(str);
}

void Insune::setSetting(const char *str, Value *value)
{
	return settings.setSetting(str, value);
}

const Style *Insune::getStyle() const
{
	return style;
}

void Insune::setStyle(Style *s)
{
	style = s;
}

int Insune::getVersion()
{
	static long gen = 0;
	if (gen == 0) {
		int i;
		char cpuinfo[256];
		char *ptr;
		FILE *file;
		
		if ((file = fopen("/proc/cpuinfo", "r")) != NULL) {
			while (fgets(cpuinfo, sizeof(cpuinfo), file) != NULL)
				if (strncmp(cpuinfo, "Revision", 8) == 0)
					break;
			fclose(file);

			for (i = 0; !isspace(cpuinfo[i]); i++);
			for (; isspace(cpuinfo[i]); i++);
			ptr = cpuinfo + i + 2;
		
			gen = strtol(ptr, NULL, 16);
		}
	}
	return gen;
}

Insune::Insune()
{
	srand(time(NULL));
	settings.load(SETTINGDIR "insune.conf");
	Setting *set;
	if ((set = settings.getSetting("style")) == NULL) {
		set = new Setting("style", new Value("default"));
		settings.addSetting(set);
	}
	char stylefile[256];
	snprintf(stylefile, 256, STYLEDIR "%s/style.ss",
			set->getValue()->getString());
	style = Style::parse(stylefile);
	implicitRedraw = false;
}

void Insune::preload()
{
	Setting *set;
	if ((set = settings.getSetting("preload")) == NULL) {
		set = new Setting("preload", new Value("header,mpdhelper"));
		settings.addSetting(set);
	}
	char *p, *dup, *nul = strdup(set->getValue()->getString());
	dup = nul;
	ApplicationFactory *appf = ApplicationFactory::getSingleton();
	while ((p = strtok(nul, ","))) {
		nul = NULL;
		Application *app = appf->openApplication(p);
		if (app)
			addApplication(app);
	}
	free(dup);
}

Insune::~Insune()
{
	settings.save();
}

void Insune::dispose()
{
	delete style;
	settings.save();
	engine.dispose();
}

void Insune::exit(int code)
{
	dispose();
	::exit(code);
}

void signalHandler(int signal)
{
	switch (signal) {
	case SIGCHLD:
		wait(NULL);
		break;
	case SIGABRT:
		fprintf(stderr, "Abort caught. Recovering by exiting.");
		Insune::getSingleton()->exit(0);
		break;
	}
}

void signalSetup()
{
	struct sigaction act;
	act.sa_handler = signalHandler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGABRT, &act, NULL);
	sigaction(SIGCHLD, &act, NULL);
}


int main(int argc, char **argv)
{
#ifdef UNITTESTS
	if (!unitTest())
		return 1;
#endif
	signalSetup();
	try {
		Hardware::init(argc, argv);
		Insune *insune = Insune::getSingleton();
		insune->preload();
		insune->addApplication(new MainApplication());

		for (;;) {
			insune->event();
			insune->update();
			insune->draw();
#ifndef IPOD
			usleep(50000);
#endif
		}
	} catch (Exception &e) {
		fprintf(stderr, "Terminating on error: %s\n", e.getMessage());
		fprintf(stderr, "Backtrace:\n");
		e.printStackTrace();
	} catch (...) {
		fprintf(stderr, "Terminating on exception; no error\n");
	}
	return EXIT_FAILURE;
}
