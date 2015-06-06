#include <string.h>
#include <stdlib.h>
#include "album.h"

Album::Album(Artist *artist, const char *name)
 : name(0)
{
	setName(name);
	setArtist(artist);
}

Album::~Album()
{
	if (name)
		free(name);
}

const char *Album::getName() const
{
	return name;
}

Artist *Album::getArtist() const
{
	return artist;
}

void Album::setName(const char *name)
{
	if (this->name)
		free(this->name);
	this->name = name ? strdup(name) : 0;
}

void Album::setArtist(Artist *artist)
{
	this->artist = artist;
}
