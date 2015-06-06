#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "widget.h"

enum DialogType
{
	DIALOG_BASIC,
	DIALOG_OK,
	DIALOG_YESNO,
};

enum DialogResult
{
	DIALOG_ACCEPT,
	DIALOG_CANCEL
};

class DialogWidget : public Widget
{
 public:
	DialogWidget(const char *message, DialogType type);
	~DialogWidget();

	DialogResult getResult() const;
	void event(Event &e);
	void draw(Canvas &canvas);
 private:
	DialogResult result;
	DialogType type;
	char *message;
};

class Dialog : public Application
{
 public:
	Dialog(const char *message, DialogType type);
	~Dialog();

	DialogResult getResult() const;

 private:
 	DialogWidget *widget;
};

#endif
