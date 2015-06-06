#include "MPDInterface.h"
#include "exception.h"
#include <string.h>

#define err() fireErrorOccured(MusicInterface::Error(connection.errorCode, connection.errorStr))
static const char *MagicAll = "All Albums";

MPDInterface::MPDInterface()
 : host(NULL), port(-1), password(NULL), timeout(16),
   connected(false), currentSong(new Album(new Artist(0), 0),0,0)
{ }

MPDInterface::MPDInterface(const char *h, int p, const char *pass)
 : host(NULL), port(-1), password(NULL), timeout(16),
   connected(false), currentSong(new Album(new Artist(0), 0),0,0)
{
	setHost(h);
	setPort(port);
	setPassword(pass);
}

MPDInterface::~MPDInterface()
{
	if (connected)
		disconnect();
	setHost(NULL);
	setPassword(NULL);
	delete currentSong.getAlbum()->getArtist();
	delete currentSong.getAlbum();
	mpd_freeInfoEntityInfo_st();
}

void MPDInterface::setHost(const char *host)
{
	if (this->host)
		free(this->host);
	this->host = host ? strdup(host) : 0;
}

void MPDInterface::setPort(int port)
{
	this->port = port;
}

void MPDInterface::setPassword(const char *password)
{
	if (this->password)
		free(this->password);
	this->password = password ? strdup(password) : 0;
}

void MPDInterface::connect()
{
	if (!host) {
		if (!(host = getenv("MPD_HOST")))
			host = strdup("127.0.0.1");
		else
			host = strdup(host);
	}
	if (port == -1) {
		char *portstring = getenv("MPD_PORT");
		if (portstring)
			port = atoi(portstring);
		else
			port = 6600;
	}

	mpd_newConnection_st(&connection, host, port, timeout);
	memset(&lastStatus, 0, sizeof(mpd_Status));
	lastStatus.error = (char *)0x123;
	if (connection.error)
		err();
	else
		connected = true;
}

void MPDInterface::disconnect()
{
	mpd_closeConnection(&connection);
	connected = false;
}

void MPDInterface::update()
{
	if (connection.error) {
		err();
		mpd_clearError(&connection);
	}
	
	mpd_Status status;
	status.error = NULL;
	status.songid = 0;
	mpd_sendStatusCommand(&connection);
	mpd_getStatus_st(&status, &connection);
	mpd_finishCommand(&connection);
	if (connection.error) {
		err();
		return;
	}


#define STATUS(v) StatusChange(MusicInterface::StatusChange::STATUS_##v)
	if (status.error == NULL) {
		if (lastStatus.volume != status.volume) {
			statusH.setVolume(status.volume);
			fireStatusChanged(STATUS(VOLUME));
		}
		if (lastStatus.repeat != status.repeat) {
			statusH.setRepeat(status.repeat);
			fireStatusChanged(STATUS(REPEAT));
		}
		if (lastStatus.random != status.random) {
			statusH.setShuffle(status.random);
			fireStatusChanged(STATUS(SHUFFLE));
		}
		if (lastStatus.state != status.state) {
			statusH.setPlaying(status.state==MPD_STATUS_STATE_PLAY);
			statusH.setPaused(status.state==MPD_STATUS_STATE_PAUSE);
			fireStatusChanged(STATUS(STATE));
		}
		if (lastStatus.playlist != status.playlist) {
			fireStatusChanged(STATUS(PLAYLIST));
		}
		if (lastStatus.songid != status.songid) {
			getCurrentSong();
			statusH.setSong(&currentSong);
			fireStatusChanged(STATUS(SONG));
		}
	}
	statusH.setTime(status.elapsedTime);
	statusH.setTotalTime(status.totalTime);
	memcpy(&lastStatus, &status, sizeof(mpd_Status));
}

void MPDInterface::setConnectionTimeout(int n)
{
	timeout = n;
}

void MPDInterface::next()
{
	mpd_sendNextCommand(&connection);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::prev()
{
	mpd_sendPrevCommand(&connection);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::play(int start)
{
	mpd_sendPlayCommand(&connection, start);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::pause()
{
	mpd_sendPauseCommand(&connection, 1);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::stop()
{
	mpd_sendStopCommand(&connection);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::seek(int pos)
{
	mpd_sendSeekIdCommand(&connection, lastStatus.songid, pos);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::queueSong(Song *song)
{
	mpd_sendAddCommand(&connection, song->getPath());
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::queuePlaylist(Playlist *plst)
{
	clearQueue();
	mpd_sendLoadCommand(&connection, plst->getName());
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::clearQueue()
{
	mpd_sendClearCommand(&connection);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

list<Artist *> *MPDInterface::getArtists(const char *search)
{
	if (search == NULL)
		search = "";
	mpd_sendListCommand(&connection, MPD_TABLE_ARTIST, search);
	if (connection.error) {
		err();
		return NULL;
	}

	char *artist;
	list<Artist *> *artists = new list<Artist *>();
	while ((artist = mpd_getNextArtist(&connection))) {
		artists->append(new Artist(artist));
		free(artist);
	}
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
	return artists;
}

list<Album *> *MPDInterface::getAlbums(Artist *artist)
{
	const char *search = artist ? artist->getName() : "";
	mpd_sendListCommand(&connection, MPD_TABLE_ALBUM, search);
	if (connection.error) {
		err();
		return NULL;
	}

	char *album;
	list<Album *> *albums = new list<Album *>();
	while ((album = mpd_getNextAlbum(&connection))) {
		albums->append(new Album(artist, album));
		free(album);
	}
	mpd_finishCommand(&connection);
	if (connection.error)
		err();

	if (artist)
		albums->prepend(new Album(artist, MagicAll));
	return albums;
}

list<Song *> *MPDInterface::getSongs(Album *album)
{
	mpd_InfoEntity entity;
	bool byAlbum = false;
	
	if (album == NULL)
		mpd_sendListallInfoCommand(&connection, "");
	else if ((album->getName() == NULL||!strcmp(album->getName(), MagicAll))
			&& album->getArtist() != NULL)
		mpd_sendFindCommand(&connection, MPD_TABLE_ARTIST,
				album->getArtist()->getName());
	else if (album->getArtist() == NULL)
		mpd_sendFindCommand(&connection, MPD_TABLE_ALBUM,
				album->getName());
	else {
		mpd_sendFindArtistAlbum(&connection,
				album->getArtist()->getName(), album->getName());
		byAlbum = true;
	}
	if (connection.error) {
		err();
		return NULL;
	}
	list<Song *> *songs = new list<Song *>();
	while ((mpd_getNextInfoEntity_st(&entity, &connection))) {
		if (entity.type != MPD_INFO_ENTITY_TYPE_SONG)
			continue;
		mpd_Song *msong = entity.info.song;
		Song *song = new Song(album, msong->title, msong->file);
		if (msong->track)
			song->setTrack(atoi(msong->track));
		songs->append(song);
	}
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
	return songs;
}

list<Song *> *MPDInterface::getQueue()
{
	mpd_sendPlaylistInfoCommand(&connection, -1);
	if (connection.error) {
		err();
		return NULL;
	}

	mpd_InfoEntity entity;
	list<Song *> *songs = new list<Song *>();
	while ((mpd_getNextInfoEntity_st(&entity, &connection))) {
		if (entity.type != MPD_INFO_ENTITY_TYPE_SONG)
			continue;
		mpd_Song *msong = entity.info.song;
		songs->append(new Song(NULL, msong->title, msong->file));
	}
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
	return songs;
}

list<Playlist *> *MPDInterface::getPlaylists()
{
	mpd_InfoEntity entity;
	mpd_sendLsInfoCommand(&connection, "");

	list<Playlist *> *playlists = new list<Playlist *>();
	while ((mpd_getNextInfoEntity_st(&entity, &connection))) {
		mpd_PlaylistFile *playlist;
		if (entity.type != MPD_INFO_ENTITY_TYPE_PLAYLISTFILE)
			continue;
		playlist = entity.info.playlistFile;
		playlists->append(new Playlist(playlist->path));
	}
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
	return playlists;
}

const MusicInterface::Status &MPDInterface::getStatus()
{
	try {
		statusH.getSong();
		// Current song set.
	} catch (Exception &) {
		// Update current song, to reduce unnecessary exceptions.
		getCurrentSong();
		statusH.setSong(&currentSong);
	}
	return statusH;
}

const Song &MPDInterface::getCurrentSong()
{
	mpd_InfoEntity entity;
	mpd_sendCurrentSongCommand(&connection);
	while ((mpd_getNextInfoEntity_st(&entity, &connection))) {
                if (entity.type != MPD_INFO_ENTITY_TYPE_SONG)
                        continue;
		mpd_Song *msong = entity.info.song;
		currentSong.setName(msong->title);
		currentSong.setPath(msong->file);
		currentSong.getAlbum()->setName(msong->album);
		currentSong.getAlbum()->getArtist()->setName(msong->artist);
		break;
        }
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
	return currentSong;
}

void MPDInterface::setVolume(int v)
{
	mpd_sendSetvolCommand(&connection, v);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::setRepeat(bool set)
{
	mpd_sendRepeatCommand(&connection, (int)set);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::setShuffle(bool set)
{
	mpd_sendRandomCommand(&connection, (int)set);
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}

void MPDInterface::updateDatabase()
{
	mpd_sendUpdateCommand(&connection, "/");
	mpd_finishCommand(&connection);
	if (connection.error)
		err();
}
