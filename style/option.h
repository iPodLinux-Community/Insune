#ifndef _OPTION_H_
#define _OPTION_H_

#include "list.h"
#include "appearance.h"
#include "visual.h"

class Option
{
 public:
	Option();
	Option(const char *tag, const char *value = 0);
	~Option();

	void setValue(char *value);
	const char *getValue() const;
	const char *getTag() const;
	const Visual &getVisual();
	const Rect getGeometry(const Rect &ref) const;
	const Padding getPadding() const;
	bool getBoolean() const;

 private:

	Appearance *parseAppearance(char *str);
	void setVisual(char *str);
	char *tag;
	char *value;
	Visual appearances;
};

Color atoc(const char *str);

#endif
