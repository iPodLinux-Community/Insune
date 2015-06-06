#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

class Playlist
{
 public:
	Playlist(const char *name);
	~Playlist();

	const char *getName() const;

 private:
	char *name;
};

#endif
