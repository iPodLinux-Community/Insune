#ifndef _MPDPLAYER_H_
#define _MPDPLAYER_H_

#include "font.h"
#include "widget.h"
#include "label.h"
#include "MusicInterface.h"
#include "image.h"

class MPDPlayer : public Widget , public MusicInterface::StatusListener
{
 public:
	MPDPlayer(MusicInterface &intr, Canvas *);
	~MPDPlayer();

	void draw(Canvas &);
	void event(Event &);
	void getArtwork();
	void statusChanged(const MusicInterface::StatusChange &status);

 private:
	void seekTimeout(int);
	void volumeTimeout(int);
	void moveSomething(int d);

	MusicInterface &mpd;
	Song *currentSong;
	Image *image;
	Font *font;
	bool imageResized;

	enum Mode {
		MODE_IDLE,
		MODE_VOLUME,
		MODE_SEEK
	} sMode;
	Timer *sTimer;
	Timer *timer;
	Label artistLabel;
	Label albumLabel;
	Label songLabel;
};

#endif
