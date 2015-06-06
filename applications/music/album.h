#ifndef _ALBUM_H_
#define _ALBUM_H_

#include "artist.h"

class Album
{
 public:
	Album(Artist *artist, const char *name);
	~Album();

	const char *getName() const;
	Artist *getArtist() const;

	void setName(const char *);
	void setArtist(Artist *);

 private:
 	Artist *artist;
	char *name;
};

#endif
