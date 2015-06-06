#ifndef _MUSICINTERFACE_H_
#define _MUSICINTERFACE_H_
#include "action.h"
#include "list.h"
#include "song.h"
#include "playlist.h"

class MusicInterface
{
 public:
	class Error
	{
	 public:
	 	Error(int code, const char *);
		~Error();
		int getCode() const;
		const char *getMessage() const;
	 private:
		int code;
		char *message;
	};

	class Status
	{
	 public:
		Status();
		
		int getVolume() const;
		bool getShuffle() const;
		bool getRepeat() const;
		bool isPlaying() const;
		bool isPaused() const;
		const Song &getSong() const;

		int getTime() const;
		int getTotalTime() const;

		void setVolume(int);
		void setShuffle(bool);
		void setRepeat(bool);
		void setPlaying(bool);
		void setPaused(bool);
		void setSong(const Song *);
		void setTime(int);
		void setTotalTime(int);
	 private:
		int volume;
		bool shuffle;
		bool repeat;
		bool playing;
		bool paused;
		const Song *song;
		int time;
		int totalTime;
	};

	class StatusChange
	{
	 public:
	 	enum Status {
			STATUS_VOLUME,
			STATUS_REPEAT,
			STATUS_SHUFFLE,
			STATUS_STATE,
			STATUS_PLAYLIST,
			STATUS_SONG
		};
		StatusChange(Status s);
		Status getStatus() const;

	 private:
		Status status;
	};

	class ErrorListener
	{
	 public:
	 	virtual ~ErrorListener() {}
		virtual void errorOccured(const Error &) = 0;
	};

	class StatusListener
	{
	 public:
		virtual ~StatusListener() {}
		virtual void statusChanged(const StatusChange &) = 0;
	};

	MusicInterface();
	virtual ~MusicInterface();

	static int RESUME;

	virtual void play(int start = RESUME) = 0;
	virtual void pause() = 0;
	virtual void next() = 0;
	virtual void prev() = 0;
	virtual void stop() = 0;
	virtual void seek(int pos) = 0;

	virtual void setVolume(int volume) = 0;
	virtual void setRepeat(bool set) = 0;
	virtual void setShuffle(bool set) = 0;

	virtual void queueSong(Song *) = 0;
	virtual void queuePlaylist(Playlist *) = 0;
	virtual void clearQueue() = 0;

	virtual list<Artist *> *getArtists(const char *search = NULL) = 0;
	virtual list<Album *> *getAlbums(Artist *artist) = 0;
	virtual list<Song *> *getSongs(Album *album) = 0;
	virtual list<Song *> *getQueue() = 0;
	virtual list<Playlist *> *getPlaylists() = 0;

	virtual void update() = 0;
	virtual const Status &getStatus() = 0;

	void registerErrorListener(ErrorListener *);
	void registerStatusListener(StatusListener *);
	void unregisterErrorListener(ErrorListener *);
	void unregisterStatusListener(StatusListener *);

 protected:
 	void fireErrorOccured(const Error &);
	void fireStatusChanged(const StatusChange &);

 private:
	list<ErrorListener *> errorListeners;
	list<StatusListener *> statusListeners;
};

#endif
