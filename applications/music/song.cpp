#include <string.h>
#include <stdlib.h>
#include "song.h"

Song::Song()
 : album(0), name(0), path(0)
{
	this->track = 0;
}

Song::Song(Album *album, const char *name, const char *path)
 : name(0), path(0)
{
	setAlbum(album);
	setName(name);
	setPath(path);
	this->track = 0;
}

Song::~Song()
{
	if (name)
		free(name);
	if (path)
		free(path);
}

const char *Song::getName() const
{
	return name;
}

Album *Song::getAlbum() const
{
	return album;
}

const char *Song::getPath() const
{
	return path;
}

int Song::getTrack() const
{
	return track;
}

void Song::setName(const char *name)
{
	if (this->name)
		free(this->name);
	this->name = name ? strdup(name) : 0;
}

void Song::setAlbum(Album *a)
{
	album = a;
}

void Song::setPath(const char *path)
{
	if (this->path)
		free(this->path);
	this->path = path ? strdup(path) : 0;
}

void Song::setTrack(int track)
{
	this->track = track;
}
