#ifndef _STYLE_H_
#define _STYLE_H_

#include "option.h"
#include "list.h"

class Style
{
 public:
	static Style *parse(const char *filename);
	const char *getName() const;
	void setName(const char *name);

	Option *resolve(const char *re) const;
	const Style *findStyle(const char *re) const;
	Option *getOption(const char *tag) const;

	~Style();

 private:
	Style(const char *name);

	static Style *parseSection(char *name, char *buf, long len);

	char *name;
	list<Option *> options;
	list<Style *> children;
};

#endif /* _STYLE_H_ */
