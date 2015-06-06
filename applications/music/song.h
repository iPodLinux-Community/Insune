#ifndef _SONG_H_
#define _SONG_H_

#include "album.h"

class Song
{
 public:
	Song();
	Song(Album *album, const char *name, const char *path);
	~Song();

	const char *getName() const;
	const char *getPath() const;
	Album *getAlbum() const;
	int getTrack() const;

	void setName(const char *);
	void setPath(const char *);
	void setAlbum(Album *al);
	void setTrack(int);

 private:
	Album *album;
	char *name;
	char *path;
	int track;
};

#endif
