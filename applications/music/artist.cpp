#include <stdlib.h>
#include <string.h>
#include "artist.h"

Artist::Artist(const char *name)
 : name(0)
{
	setName(name);
}

void Artist::setName(const char *name)
{
	if (this->name)
		free(this->name);
	this->name = name ? strdup(name) : 0;
}

Artist::~Artist()
{
	if (name)
		free(name);
}

const char *Artist::getName() const
{
	return name;
}

