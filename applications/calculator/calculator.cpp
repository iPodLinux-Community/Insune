#include <unistd.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#include "insune.h"
#include "application.h"

#define DIV  16
#define MULT 17
#define SUBT 18
#define ADD  19
#define EQL  20
#define DP   21
#define CLR  22
#define POW  23
#define POW2 24
#define SQRT 25

#define WRAP(val, max) (val = ((val > max)?0:(val < 0)?max:val))

struct _button {
	char ch[5];
	unsigned char math;
} buttons[] = {
	{"7", 7}, {"8", 8},  {"9", 9},   {"/", DIV},  {"C", CLR},
	{"4", 4}, {"5", 5},  {"6", 6},   {"*", MULT}, {"^", POW},
	{"1", 1}, {"2", 2},  {"3", 3},   {"-", SUBT}, {"^2", POW2},
	{"0", 0}, {".", DP}, {"=", EQL}, {"+", ADD},  {"sqrt", SQRT},
};

#define ROWS 4
#define COLS 5

#define FIELD_HEIGHT 40

class CalculatorWidget : public Widget
{
 public:
	CalculatorWidget(Canvas *canvas)
	 : Widget(canvas), font(NULL), selected(0),
		e(0), op(0), number(0), last(0)
	{ }

	~CalculatorWidget()
	{ }

	void setFont(const char *fontname)
	{
		font = FontFactory::getSingleton()->loadFont(fontname);
	}

	void draw(Canvas &canvas)
	{
		const Style *style = Insune::getSingleton()->getStyle();
		Option *o;

		canvas.clear();
		Rect rect = Rect(0, 0, getSize().width(), getSize().height());
		if ((o = style->resolve("calculator/background")))
			canvas.drawVisual(o->getVisual(), rect);
		else {
			canvas.setColor(atoc("#40000000"));
			canvas.fillRectangle(rect);
		}

		rect = Rect(1, 1, getSize().width() - 1, FIELD_HEIGHT - 1);
		if ((o = style->resolve("calculator/field/background")))
			canvas.drawVisual(o->getVisual(), rect);
		else {
			canvas.setColor(0xade1fe);
			canvas.drawRectangle(rect);
		}

		if ((o = style->resolve("calculator/field/color")))
			canvas.setColor(atoc(o->getValue()));
		else
			canvas.setColor(0xff000000);

		char value[256];
		snprintf(value, 256, "%f", number);

		canvas.setFont(font);
		canvas.drawText(getSize().width() - font->width(value) - 5, FIELD_HEIGHT / 2 - font->height() / 2, value);

		for (int row = 0; row < ROWS; row++){
			for (int col = 0; col < COLS; col++){
				drawButton(canvas, row, col);
			}
		}
	}

	void event(Event &e)
	{
		switch (e.type) {
		case KEY_DOWN:
			setDirty(true);
			switch (e.key.value) {
			case SCROLL_LEFT:
				selected--;
				WRAP(selected, ROWS * COLS - 1);
				return;
			case SCROLL_RIGHT:
				selected++;
				WRAP(selected, ROWS * COLS - 1);
				return;
			case KEY_ACTION:
				calculate();
				return;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

 private:
	void drawButton(Canvas &canvas, int row, int col)
	{
		const char *background[] = {
			"calculator/button/background",
			"calculator/button!selected/background"
		};
		const char *color[] = {
			"calculator/button/color",
			"calculator/button!selected/color"
		};
		const Style *style = Insune::getSingleton()->getStyle();
		Option *o;

		int w  = getSize().width() / COLS;
		int h  = (getSize().height() - FIELD_HEIGHT) / ROWS;
		int x  = 1 + col * w;
		int y  = FIELD_HEIGHT + row * h;
		int p  = col + row * COLS;

		Rect rect = Rect(x, y, x + w - 1 - (col == COLS - 1), y + h - 1);
		if ((o = style->resolve(background[(p == selected)])))
			canvas.drawVisual(o->getVisual(), rect);
		else {
			canvas.setColor((p == selected)?0x505050:0x3f3f3f);
			canvas.drawRectangle(rect);
		}

		if ((o = style->resolve(color[(p == selected)])))
			canvas.setColor(atoc(o->getValue()));
		else
			canvas.setColor((p == selected)?0xffff9000:0xffffffff);

		canvas.drawText(x + w / 2 - font->width(buttons[p].ch) / 2, y + h / 2 - font->height() / 2, buttons[p].ch);
	}

	double lpow(double x, signed char p)
	{
		double i = x;

		if (p > 0) while (--p > 0) x *= i;
		else if (p-- < 0) while(p++ < 0) x /= i;
		else return 1;

		return x;
	}

	double operation(double a, unsigned char op, double b)
	{
		switch (op) {
			case DIV:  return a / b;
			case MULT: return a * b;
			case SUBT: return a - b;
			case ADD:  return a + b;
			case POW:  return pow(a, b);
		}

		return 0;
	}

	void calculate()
	{
		struct _button *p = &buttons[selected];
		stump = 0;

		if (p->math & 16) {
			switch (p->math) {
				case DIV:
				case MULT:
				case SUBT:
				case ADD:
				case POW:
					if (op) number = operation(last, op, number);
					op     = p->math;
					last   = number;
					stump  = 1;
					e      = 0;
					number = 0;
					break;
				case SQRT:
					number = sqrt(number);
					stump  = 1;
					e      = 0;
					break;
				case POW2:
					number = pow(number, 2);
					stump  = 1;
					e      = 0;
					break;
				case EQL:
					number = operation(last, op, number);
					op     = 0;
					stump  = 1;
					e      = 0;
					break;
				case CLR:
					number = 0;
					op     = 0;
					stump  = 1;
					e      = 0;
					break;
				case DP:
					if (!e) --e;
					break;
			}
		}
		else if (!stump) {
			if (!e) number *= 10;
			number += p->math * lpow(10, e);
			if (e < 0) --e;
		}
		else {
			number = p->math;
			stump  = 0;
		}
	}

 private:
	Font *font;
	int selected;
	int stump;
	signed char e;
	unsigned char op;
	double number;
	double last;
};



class Calculator : public Application
{
 public:
	Calculator();
	~Calculator();

	void event(Event &e);

 private:
	CalculatorWidget *calculator;
};

Calculator::Calculator()
{
	Canvas *canvas = new DefaultCanvas();
	canvas->setBackground(HD_CLEAR);

	calculator = new CalculatorWidget(canvas);
	calculator->setFont("Aiken14");

	addWidget(calculator);
}

Calculator::~Calculator()
{
	Insune *insune = Insune::getSingleton();
	removeWidget(calculator);
	insune->removeApplication(this);
	delete calculator;
}

void Calculator::event(Event &e)
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

APPLICATION(Calculator)

