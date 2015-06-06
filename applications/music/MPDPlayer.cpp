#include <unistd.h>
#include <string.h>
#include "insune.h"
#include "style.h"
#include "exception.h"
#include "MPDPlayer.h"

MPDPlayer::MPDPlayer(MusicInterface &intr, Canvas *canvas)
 : Widget(canvas), mpd(intr), imageResized(false),
    sMode(MODE_IDLE), sTimer(0)
{
	addTimer((timer = new Timer(500, new Action<MPDPlayer, bool>(this, &MPDPlayer::setDirty, true))));

	addWidget(&artistLabel);
	addWidget(&albumLabel);
	addWidget(&songLabel);

	Option *o;
	const Style *style = Insune::getSingleton()->getStyle();
	if ((o = style->resolve("music/player/song/font"))) {
		FontFactory *ff = FontFactory::getSingleton();
		font = ff->loadFont(o->getValue());
		artistLabel.setFont(font);
		albumLabel.setFont(font);
		songLabel.setFont(font);
	}
	if ((o = style->resolve("music/player/song/color"))) {
		Color col = atoc(o->getValue());
		artistLabel.setColor(col);
		albumLabel.setColor(col);
		songLabel.setColor(col);
	}
	if ((o = style->resolve("music/player/song/align"))) {
		Label::Alignment align;
		if (strstr(o->getValue(), "left"))
			align = Label::ALIGN_LEFT;
		else if (strstr(o->getValue(), "right"))
			align = Label::ALIGN_RIGHT;
		else if (strstr(o->getValue(), "center"))
			align = Label::ALIGN_CENTER;
		else
			align = Label::ALIGN_LEFT;
		artistLabel.setAlignment(align);
		albumLabel.setAlignment(align);
		songLabel.setAlignment(align);
	}

	if ((o = style->resolve("music/player/song/geometry"))) {
		Rect rect = o->getGeometry(Rect(Position(0,0), getSize()));
		artistLabel.setSize(Size(rect.width(), rect.height()));
		albumLabel.setSize(Size(rect.width(), rect.height()));
		songLabel.setSize(Size(rect.width(), rect.height()));
	}
	if ((o = style->resolve("music/player/song/scroll"))) {
		bool scroll = o->getBoolean();
		artistLabel.setScroll(scroll);
		albumLabel.setScroll(scroll);
		songLabel.setScroll(scroll);
	}

	image = NULL;

	mpd.registerStatusListener(this);
	statusChanged(MusicInterface::StatusChange(MusicInterface::StatusChange::STATUS_SONG));
}

MPDPlayer::~MPDPlayer()
{
	mpd.unregisterStatusListener(this);
	if (image)
		delete image;
	removeTimer(timer);
	delete timer;
}

/*
 * artwork files should be in /home/covers, or $HOME/.covers
 * the search order is: 
 *    <artist>-<album>.jpg
 *    <artist>-.jpg,
 *    Various Artists-<album>.jpg
 *    -<album>.jpg
 */
void MPDPlayer::getArtwork()
{
	if (image) {
		delete image;
		image = NULL;
	}
	char buf[256];

	const MusicInterface::Status &currentStatus = mpd.getStatus();
	if (!currentStatus.isPlaying() && !currentStatus.isPaused())
		return;
	const Song &song = currentStatus.getSong();
	Album *_album = song.getAlbum();
    	Artist *_artist = _album->getArtist();

	const char *artist = _artist->getName();
	if (artist == NULL) return;
	const char *album = _album->getName();
	if (album == NULL) album = "";
#ifndef IPOD
	char covers[256];
	snprintf(covers, 256, "%s/.covers/", getenv("HOME"));
#else
	const char *covers = "/home/covers/";
#endif
	struct search {
		const char *fmt;
		const char *s2, *s3;
	} searches[] = {
		{"%s%s-%s.jpg",              artist, album},
		{"%s%s-.jpg",                artist},
		{"%sVarious Artists-%s.jpg", album},
		{"%s-%s.jpg",                album}
	};
	bool success = false;
	for (unsigned int i = 0; i < sizeof(searches)/sizeof(searches[0]); ++i){
		if (!album[0] && i > 0)
			return;
		snprintf(buf, 256, searches[i].fmt, covers,
				searches[i].s2, searches[i].s3);
		if (!access(buf, R_OK)) {
			success = true;
			break;
		}
	}
	if (success) {
		image = Image::load(buf);
		imageResized = false;
	}
}

void MPDPlayer::statusChanged(const MusicInterface::StatusChange &status)
{
	switch (status.getStatus()) {
	case MusicInterface::StatusChange::STATUS_VOLUME:
	case MusicInterface::StatusChange::STATUS_REPEAT:
	case MusicInterface::StatusChange::STATUS_SHUFFLE:
	case MusicInterface::StatusChange::STATUS_STATE:
		break;
	case MusicInterface::StatusChange::STATUS_PLAYLIST:
		break;
	case MusicInterface::StatusChange::STATUS_SONG:
		const MusicInterface::Status &currentStatus = mpd.getStatus();
		if (!currentStatus.isPlaying() && !currentStatus.isPaused()) {
			artistLabel.setText("");
			albumLabel.setText("");
			songLabel.setText("");
			getArtwork();
			return;
		}
		try {
			const Song &song = currentStatus.getSong();
			Album *album = song.getAlbum();
    			Artist *artist = album->getArtist();
			if (artist->getName())
				artistLabel.setText(artist->getName());
			else
				artistLabel.setText("");
			if (album->getName())
				albumLabel.setText(album->getName());
			else
				albumLabel.setText("");
			if (song.getName())
				songLabel.setText(song.getName());
			else {
				const char *path = song.getPath();
				const char *file = strrchr(path, '/');
				songLabel.setText(file ? file + 1 : path);
			}

			getArtwork();
		} catch (Exception &e) {
			warn("Warning (fixme:(%s:%d)): %s\n", __FILE__, __LINE__, e.getMessage());
		}

		setDirty(true);
		break;
	}
}

void MPDPlayer::volumeTimeout(int)
{
	sMode = MODE_IDLE;
	sTimer = NULL; // deleted by the widget backend
}

void MPDPlayer::seekTimeout(int)
{
	sMode = MODE_IDLE;
	sTimer = NULL; // deleted by the widget backend
}


void MPDPlayer::moveSomething(int d)
{
	const MusicInterface::Status &status = mpd.getStatus();
	void (MPDPlayer::*func)(int);
	switch (sMode) {
	case MODE_IDLE:
		sMode = MODE_VOLUME;
		return;
	case MODE_VOLUME:
		mpd.setVolume((status.getVolume()&~1) + 2*d);
		func = &MPDPlayer::volumeTimeout;
		break;
	case MODE_SEEK:
		func = &MPDPlayer::seekTimeout;
		if (status.getTotalTime() == 0 || d == 0)
			break;
		mpd.seek((int)((float)status.getTime() + (float)status.getTotalTime()*d/100) % status.getTotalTime());
		break;
	default:
		break;
	}
	if (sTimer) {
		removeTimer(sTimer);
		delete sTimer;
	}
	sTimer = new Timer(1500, new Action<MPDPlayer, int>(this, func));
	sTimer->setSingleUse();
	addTimer(sTimer);
	setDirty(true);
}

void MPDPlayer::event(Event &e)
{
	switch (e.type) {
	case KEY_DOWN:
		switch (e.key.value) {
		case KEY_ACTION:
			sMode = MODE_SEEK;
			moveSomething(0);
			break;
		case KEY_FFORWARD:
			mpd.next();
			break;
		case KEY_REWIND:
			mpd.prev();
			break;
		case KEY_PLAY:
			if (mpd.getStatus().isPlaying())
				mpd.pause();
			else
				mpd.play();
			break;
		case SCROLL_LEFT:
			SCROLL_MOD(5);
			moveSomething(-1);
			break;
		case SCROLL_RIGHT:
			SCROLL_MOD(5);
			moveSomething(1);
			break;
		default:
			break;
		}
	default:
		break;
	}
}

void MPDPlayer::draw(Canvas &c)
{
	const Style *ss, *style = Insune::getSingleton()->getStyle();
	Option *o;
	c.clear();
	Rect window(Position(0,0), getSize());

	if (!(style = style->findStyle("music/player")))
		return;
	if ((o = style->getOption("background")))
		c.drawVisual(o->getVisual(), window);
	if ((o = style->getOption("color")))
		c.setColor(atoc(o->getValue()));
	if ((ss = style->findStyle("artwork"))) {
		Rect r = ss->getOption("geometry")->getGeometry(window);
		float rat = (image) ? ((float)image->height() / image->width()) : 1.0;
		int h = (int)(r.width() * rat + 0.5);
		Rect rctfy(r.left(),r.top(),r.right(),r.top()+h);
		if ((o = ss->getOption("background")))
			c.drawVisual(o->getVisual(), rctfy);
		if (image) {
			if (!imageResized) {
				image->scale(r.width(), h);
				imageResized = true;
			}
			c.blit(*image, rctfy);
			if ((o = ss->getOption("foreground")))
				c.drawVisual(o->getVisual(), rctfy);
		}
	}
	const MusicInterface::Status &currentStatus = mpd.getStatus();
	if (!currentStatus.isPlaying() && !currentStatus.isPaused()) return;

	if ((ss = style->findStyle("song"))) {
		Rect rect = ss->getOption("geometry")->getGeometry(window);
		if ((o = ss->getOption("background")))
			c.drawVisual(o->getVisual(), rect);

		if ((o = ss->getOption("font")))
			font = FontFactory::getSingleton()->loadFont(o->getValue());
		else
			font = FontFactory::getSingleton()->getDefaultFont();

		try {
			const Song &song = currentStatus.getSong();
			Album *album = song.getAlbum();
    			Artist *artist = album->getArtist();
			int mod = (rect.height() + font->height()) /
				((artist->getName() ? 1 : 0)
				+ (album->getName() ? 1 : 0) + 2);
			int num = 0;

#define moveLabel(l, m, n, f)                                   \
    l.setPosition(Position(rect.left(), rect.top() + m*(++n) + f->height()))


			if (artist->getName())
				moveLabel(artistLabel, mod, num, font);
			if (album->getName())
				moveLabel(albumLabel, mod, num, font);
			moveLabel(songLabel, mod, num, font);
		} catch (Exception &e) {
			// Song not set, yet :/
		}
	}

	if ((sMode == MODE_SEEK && (ss = style->findStyle("seekbar"))) ||
		       	(ss = style->findStyle("progressbar"))) {
		Rect rect = ss->getOption("geometry")->getGeometry(window);
		if ((o = ss->getOption("background")))
			c.drawVisual(o->getVisual(), rect);
		if ((o = ss->getOption("fill"))) {
			int total, current;
			if (sMode == MODE_VOLUME) {
				total = 100;
				current = currentStatus.getVolume();
			}
			else {
				total = currentStatus.getTotalTime();
				current = currentStatus.getTime();
				if (total == 0) return;
			}
			int width = rect.width() * current / total;
			c.drawVisual(o->getVisual(),
					Rect(rect.left(), rect.top(),
						rect.left()+ width, rect.bottom()));
			if ((o = ss->getOption("color")))
				c.setColor(atoc(o->getValue()));
			if ((o = ss->getOption("font")))
				font = FontFactory::getSingleton()->loadFont(o->getValue());
			else
				font = FontFactory::getSingleton()->getDefaultFont();
			c.setFont(font);
			char buf[256];
			if (sMode == MODE_VOLUME)
				snprintf(buf, 256, "%d%%", current*100/total);
			else
				snprintf(buf, 256, "%d:%02d / %d:%02d",
						current/60, current%60,
						total/60, total%60);
    			c.drawText(rect.left() + (rect.width() - font->width(buf))/2, rect.top() + (rect.height() - font->height())/2, buf);
		}
	}
}
