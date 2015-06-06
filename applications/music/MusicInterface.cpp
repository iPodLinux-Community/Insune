#include "MusicInterface.h"
#include "exception.h"
#include <string.h>

int MusicInterface::RESUME = -1;

MusicInterface::Error::Error(int code, const char *message)
{
	this->code = code;
	this->message = strdup(message);
}

MusicInterface::Error::~Error()
{
	free(message);
}

int MusicInterface::Error::getCode() const
{
	return code;
}

const char *MusicInterface::Error::getMessage() const
{
	return message;
}

MusicInterface::Status::Status()
 : song(0)
{ }

int MusicInterface::Status::getVolume() const
{ return volume; }
bool MusicInterface::Status::getShuffle() const
{ return shuffle; }
bool MusicInterface::Status::getRepeat() const
{ return repeat; }
bool MusicInterface::Status::isPlaying() const
{ return playing; }
bool MusicInterface::Status::isPaused() const
{ return paused; }
const Song &MusicInterface::Status::getSong() const
{
	if (!song)
		throw Exception("Song not set");
	return *song;
}
int MusicInterface::Status::getTime() const
{ return time; }
int MusicInterface::Status::getTotalTime() const
{ return totalTime; }
void MusicInterface::Status::setVolume(int v)
{ volume = v; }
void MusicInterface::Status::setShuffle(bool b)
{ shuffle = b; }
void MusicInterface::Status::setRepeat(bool b)
{ repeat = b; }
void MusicInterface::Status::setPlaying(bool b)
{ playing = b; }
void MusicInterface::Status::setPaused(bool b)
{ paused = b; }
void MusicInterface::Status::setSong(const Song *s)
{ song = s; }
void MusicInterface::Status::setTime(int t)
{ time = t; }
void MusicInterface::Status::setTotalTime(int t)
{ totalTime = t; }


MusicInterface::StatusChange::StatusChange(MusicInterface::StatusChange::Status s)
{
	status = s;
}

MusicInterface::StatusChange::Status MusicInterface::StatusChange::getStatus() const
{
	return status;
}

MusicInterface::MusicInterface()
{ }
MusicInterface::~MusicInterface()
{ }

void MusicInterface::registerErrorListener(MusicInterface::ErrorListener *l)
{
	errorListeners.append(l);
}
void MusicInterface::unregisterErrorListener(MusicInterface::ErrorListener *l)
{
	errorListeners.remove(l);
}

void MusicInterface::registerStatusListener(MusicInterface::StatusListener *l)
{
	statusListeners.append(l);
}
void MusicInterface::unregisterStatusListener(MusicInterface::StatusListener *l)
{
	statusListeners.remove(l);
}

void MusicInterface::fireErrorOccured(const MusicInterface::Error &e)
{
	list<MusicInterface::ErrorListener *>::iterator it = errorListeners.start();
	for (; it != errorListeners.end(); ++it)
		(*it)->errorOccured(e);
}
void MusicInterface::fireStatusChanged(const MusicInterface::StatusChange &s)
{
	list<MusicInterface::StatusListener *>::iterator it = statusListeners.start();
	for (; it != statusListeners.end(); ++it)
		(*it)->statusChanged(s);
}
