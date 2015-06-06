#ifndef _FONT_H_
#define _FONT_H_
#include "hotdog.h"
#include "list.h"

class Font
{
 public:
	Font(const char *name, hd_font *font);
	~Font();

	hd_font *getFont();
	const char *getName() const;
	int width(const char *ch);
	int height(const char *ch);
	int height();

 private:
	hd_font *font;
	char * name;
};

class FontFactory
{
 public:
 	Font *getDefaultFont();
 	Font *loadFont(const char *font);

	static FontFactory *getSingleton();

 private:
	FontFactory();
	list<Font *> fonts;

	static FontFactory *instance;
};

#endif
