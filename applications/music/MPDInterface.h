#ifndef _MPDPROTOCOL_H_
#define _MPDPROTOCOL_H_
#include "libmpdclient.h"
#include "MusicInterface.h"
#include "action.h"
#include "list.h"
#include "song.h"
#include "playlist.h"

#define MPD_CONNECTION_ERROR -1

class MPDInterface : public MusicInterface
{
 public:
	MPDInterface();
	MPDInterface(const char * host, int port, const char *password);
	~MPDInterface();

	void setHost(const char *host);
	void setPort(int port);
	void setPassword(const char *password);
	void setConnectionTimeout(int t);

	void connect();
	void disconnect();
	void updateDatabase();

	void play(int start = MusicInterface::RESUME);
	void pause();
	void next();
	void prev();
	void stop();
	void seek(int pos);

	void setVolume(int volume);
	void setRepeat(bool set);
	void setShuffle(bool set);

	void queueSong(Song *);
	void queuePlaylist(Playlist *);
	void clearQueue();

	list<Artist *> *getArtists(const char *search = NULL);
	list<Album *> *getAlbums(Artist *artist);
	list<Song *> *getSongs(Album *album);
	list<Song *> *getQueue();
	list<Playlist *> *getPlaylists();

	void update();
	const Status &getStatus();

 private:
 	const Song &getCurrentSong();
 	char *host;
	int port;
	char *password;
	int timeout;
	bool connected;
	mpd_Connection connection;
	mpd_Status lastStatus;
	MusicInterface::Status statusH;
	Song currentSong;
};

#endif
