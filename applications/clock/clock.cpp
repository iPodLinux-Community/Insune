#include <unistd.h>
#include <time.h>
#include <math.h>

#include "insune.h"
#include "application.h"
#include "menu.h"
#include "image.h"

class ClockWidget : public Widget
{
 public:
	ClockWidget(Canvas *canvas)
	 : Widget(canvas), image(NULL)
	{
		setSize(Size(canvas->width(), canvas->height()));
		addTimer(new Timer(1000, new Action<ClockWidget, bool>(this, &ClockWidget::setDirty, true)));
	}

	~ClockWidget()
	{
		if (image)
			delete image;
	}

	void setImage(const char *filename)
	{
		if (image)
			delete image;
		image = Image::load(filename);
	}

	void setSize(const Size &si)
	{
		const Position &pos = getPosition();
		if (si.width() < si.height()) {
			int dif = si.height() - si.width();
			Widget::setSize(Size(si.width(), si.height() - dif));
			setPosition(Position(pos.X(), pos.Y() + dif/2));
		} else {
			int dif = si.width() - si.height();
			Widget::setSize(Size(si.width() - dif, si.height()));
			setPosition(Position(pos.X() + dif/2, pos.Y()));
		}
	}

	void thickLine(Canvas &canvas, int sx, int sy, int ex, int ey, int t)
	{
		t = ((t + 1) & ~1) / 2;

		for (int i = 1; i <= t; ++i) {
			canvas.drawLine(sx+i, sy  , ex+i, ey  );
			canvas.drawLine(sx-i, sy  , ex-i, ey  );
			canvas.drawLine(sx  , sy+i, ex  , ey+i);
			canvas.drawLine(sx  , sy-i, ex  , ey-i);
		}
		canvas.drawLine(sx, sy, ex, ey);
		canvas.fillCircle(sx, sy, t);
		canvas.fillCircle(ex, ey, t);
	}


	void draw(Canvas &canvas)
	{
		if (!image) return;
		canvas.clear();
		time_t timeNow;
		struct tm ti;
		time(&timeNow);
		localtime_r(&timeNow, &ti);
		int r = (getSize().height() + 1) / 2 - 2;
		int cx = r+1, cy = r+1;

		canvas.blit(*image, Rect(Position(0,0), getSize()));

		canvas.setAA(true);
		canvas.setColor(0xffff7777);
		int sx = (int)(r * cos(ti.tm_sec * M_PI/30 - M_PI_2)+0.5),
			sy = (int)(r * sin(ti.tm_sec * M_PI/30 - M_PI_2)+0.5);
		thickLine(canvas, cx, cy, cx + sx, cy + sy, 2);
		
		canvas.setColor(0xff000000);
		int hx = (int)(r/2 * cos((ti.tm_hour%12)*M_PI/6 - M_PI_2)+0.5),
			hy = (int)(r/2 * sin((ti.tm_hour%12)*M_PI/6 - M_PI_2)+0.5);
		thickLine(canvas, cx, cy, cx + hx, cy + hy, 4);

		int mx = (int)(r * cos(ti.tm_min * M_PI/30 - M_PI_2)+0.5),
			my = (int)(r * sin(ti.tm_min * M_PI/30 - M_PI_2)+0.5);
		thickLine(canvas, cx, cy, cx + mx, cy + my, 4);
	}

	void event(Event &) { }

 private:
	Image *image;
};



class Clock : public Application
{
 public:
	Clock();
	~Clock();

	void event(Event &e);

 private:
	Canvas *canvas;
	ClockWidget *clock;
};

Clock::Clock()
{
	Insune *insune = Insune::getSingleton();
	canvas = new Canvas(insune->width(), insune->height());
	clock = new ClockWidget(canvas);
	char imagename[256];
	snprintf(imagename, 256, "%s/classic.png", getDataPath());
	clock->setImage(imagename);
	addWidget(clock);
}

Clock::~Clock()
{
	Insune *insune = Insune::getSingleton();
	insune->removeApplication(this);
	removeWidget(clock);
	delete clock;
}

void Clock::event(Event &e)
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

APPLICATION(Clock)
