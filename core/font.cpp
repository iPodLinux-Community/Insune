#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "insune.h"
#include "font.h"
#include "exception.h"

FontFactory *FontFactory::instance = 0;

FontFactory::FontFactory() {}

FontFactory *FontFactory::getSingleton()
{
	if (!instance)
		instance = new FontFactory();
	return instance;
}

Font *FontFactory::getDefaultFont()
{
	Setting *set = Insune::getSingleton()->getSetting("defaultfont");
	return loadFont(set ? set->getValue()->getString() : "6x13");
}

Font *FontFactory::loadFont(const char *f)
{
	struct {
		char ext[4];
		hd_font *(* load)(const char *);
	} types[] = {
		{"hdf", HD_Font_LoadHDF},
		{"fff", HD_Font_LoadFFF},
		{"hex", HD_Font_LoadFFF},
		{"fnt", HD_Font_LoadFNT},
		{"pcf", HD_Font_LoadPCF},
		{"png", HD_Font_LoadSFont}
	};

	list<Font *>::iterator it = fonts.start();
	for (; it != fonts.end(); ++it) {
		if (!strcmp((*it)->getName(), f))
			return (*it);
	}

	char filename[256];
	int pos = snprintf(filename, 252, FONTDIR "%s.", f);
	for (unsigned int i = 0; i < sizeof(types) / sizeof(types[0]); ++i) {
		strcpy(filename + pos, types[i].ext);
		if (!access(filename, R_OK)) {
			hd_font *hfont = (* types[i].load)(filename);
			if (!hfont)
				throw Exception("Failed to load font");
			Font *font = new Font(f, hfont);
			fonts.append(font);
			return font;
		}
		if (errno != ENOENT) {
			perror(filename);
			throw Exception("No such font");
		}
	}
	throw Exception("No such font");
}

Font::Font(const char *n, hd_font *font)
{
	name = strdup(n);
	this->font = font;
}

Font::~Font()
{
	free(name);
	HD_Font_Free(font);
}

int Font::width(const char *ch)
{
	return HD_Font_TextWidth(font, ch);
}

int Font::height(const char *)
{
	return font->h;
}

int Font::height()
{
	return font->h;
}

hd_font *Font::getFont()
{
	return font;
}

const char *Font::getName() const
{
	return name;
}
