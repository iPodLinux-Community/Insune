#include <string.h>
#include <stdlib.h>
#include "playlist.h"

Playlist::Playlist(const char *n)
{
	name = strdup(n);
}

Playlist::~Playlist()
{
	free(name);
}

const char *Playlist::getName() const
{
	return name;
}
