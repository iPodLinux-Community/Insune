#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "insune.h"
#include "application.h"
#include "menu.h"
#include "groupmenu.h"
#include "exception.h"
#include "MPDInterface.h"
#include "MPDPlayer.h"

class MPDC : public Application, public MusicInterface::ErrorListener
{
 public:
	MPDC();
	~MPDC();

	void update();
	void event(Event &e);
	void errorOccured(const MusicInterface::Error &e);

 private:
	void openPlaylists(int);
	void openArtists(int);
	void openAlbums(Artist *);
	void openSongs(Album *);

	void openQueue(int);
	void openPlaying(int);

	void queueSong(Song *song);
	void queueArtist(Artist *art);
	void queueAlbum(Album *alb);

	void playSong(int);
	void playSong(Song *song);
	void playArtist(Artist *art);
	void playAlbum(Album *alb);

	char path[256];
	MPDInterface mpd;
	MenuModel *menuModel;
	list<MenuModel *> modelStack;
	GroupMenu *menu;
	MPDPlayer *player;
	bool playerActive;
};

MPDC::MPDC()
 : playerActive(false)
{
	MenuModel *menuModel = new MenuModel();

	#define ADD_MENU_ITEM(x,y,b) do{ \
		snprintf(path, 256, "%s/%s", getDataPath(), y); \
		menuModel->append(#x, Image::load(path), new Action<MPDC, b>(this, &MPDC::open##x, 0)); \
	} while(0);

	menuModel->appendGroup("Library");
	ADD_MENU_ITEM(Playlists,"table_multiple.png",int)
	ADD_MENU_ITEM(Artists,"user.png",int)
	ADD_MENU_ITEM(Albums,"cd.png",Artist*)
	
	menuModel->appendGroup("Control");
	ADD_MENU_ITEM(Queue,"script.png",int)
	ADD_MENU_ITEM(Playing,"control_play_blue.png",int)

	modelStack.push(menuModel);
	menu = new GroupMenu(menuModel, new DefaultCanvas());
	addWidget(menu);
	activate(menu);
	mpd.registerErrorListener(this);
	mpd.connect();
}

MPDC::~MPDC()
{
	Insune *insune = Insune::getSingleton();
	if (playerActive) {
		removeWidget(player);
		delete player;
	}
	mpd.unregisterErrorListener(this);
	insune->removeApplication(this);
	removeWidget(menu);
	delete modelStack.pop();
	delete menu;
}


void MPDC::errorOccured(const MusicInterface::Error &e)
{
	warn("MPDC: %s\n", e.getMessage());
	unload();
}

void MPDC::openPlaying(int)
{
	//throw Exception("Not fully implemented; nice test of exceptions though");
	player = new MPDPlayer(mpd, new DefaultCanvas());
	addWidget(player);
	activate(player);
	playerActive = true;
}

void MPDC::openPlaylists(int)
{
	MenuModel *model = new MenuModel();
	list<Playlist *> *playlists = mpd.getPlaylists();	
	list<Playlist *>::iterator it = playlists->start();
	for (; it != playlists->end(); ++it) {
		Action<MPDInterface, Playlist*> * action =
				new Action<MPDInterface, Playlist*>();
		Playlist *plst = (*it);
		action->setClass(&mpd);
		action->setMethod(&MPDInterface::queuePlaylist);
		action->setData(plst);
		model->append(plst->getName(), action);
	}
	modelStack.push(model);
	menu->doSubmenu(model);
	delete playlists;
}

void MPDC::queueSong(Song *song)
{
	mpd.queueSong(song);
}

void MPDC::playSong(int n)
{
	mpd.play(n);
	openPlaying(0);
}

void MPDC::playSong(Song *song)
{
	mpd.clearQueue();
	queueSong(song);
	mpd.play();
	openPlaying(0);
}

void MPDC::queueAlbum(Album *alb)
{
	list<Song *> *songs = mpd.getSongs(alb);
	list<Song *>::iterator it = songs->start();
	for (; it != songs->end(); ++it) {
		Song *song = (*it);
		queueSong(song);
	}
	songs->deleteAll();
	delete songs;
	
}

void MPDC::playAlbum(Album *alb)
{
	mpd.clearQueue();
	queueAlbum(alb);
	mpd.play(menu->getSelectedItem());
	openPlaying(0);
}

void MPDC::queueArtist(Artist *art)
{
	list<Album *> *albums = mpd.getAlbums(art);
	list<Album *>::iterator it = albums->start();
	for (; it != albums->end(); ++it) {
		Album *alb = (*it);
		queueAlbum(alb);
	}
	albums->deleteAll();
	delete albums;
}

void MPDC::playArtist(Artist *art)
{
	mpd.clearQueue();
	queueArtist(art);
	mpd.play();
	openPlaying(0);
}

void MPDC::openArtists(int)
{
	MenuModel *model = new MenuModel();
	list<Artist *> *artists = mpd.getArtists();	
	list<Artist *>::iterator it = artists->start();
	for (; it != artists->end(); ++it) {
		Action<MPDC, Artist*> * action = new Action<MPDC, Artist*>();
		Artist *art = (*it);
		action->setClass(this);
		action->setMethod(&MPDC::openAlbums);
		action->setData(art);
		model->append(art->getName(), action);
	}
	modelStack.push(model);
	menu->doSubmenu(model);
	delete artists;
}

void MPDC::openAlbums(Artist *art)
{
	MenuModel *model = new MenuModel();
	list<Album *> *albums = mpd.getAlbums(art);
	list<Album *>::iterator it = albums->start();
	for (; it != albums->end(); ++it) {
		Action<MPDC, Album*> * action = new Action<MPDC, Album*>();
		Album *alb = (*it);
		action->setClass(this);
		action->setMethod(&MPDC::openSongs);
		action->setData(alb);
		model->append(alb->getName(), action);
	}
	modelStack.push(model);
	menu->doSubmenu(model);
	delete albums;
}

void MPDC::openSongs(Album *alb)
{
	MenuModel *model = new MenuModel();
	list<Song *> *songs = mpd.getSongs(alb);
	list<Song *>::iterator it = songs->start();
	for (int i = 0; it != songs->end(); ++it, ++i) {
		Song *song = (*it);
		Action<MPDC, Album*> * action = new Action<MPDC, Album*>();
		action->setClass(this);
		action->setMethod(&MPDC::playAlbum);
		action->setData(alb);

		const char *name = song->getName() ?
			song->getName() : song->getPath();
		model->append(name, action);
	}
	modelStack.push(model);
	menu->doSubmenu(model);
	delete songs;
}

void MPDC::openQueue(int)
{
	MenuModel *model = new MenuModel();
	list<Song *> *songs = mpd.getQueue();
	list<Song *>::iterator it = songs->start();
	for (int i = 0; it != songs->end(); ++it, ++i) {
		Song *song = (*it);
		Action<MPDC, int> * action = new Action<MPDC, int>();
		action->setClass(this);
		action->setMethod(&MPDC::playSong);
		action->setData(i);

		const char *name = song->getName() ?
			song->getName() : song->getPath();
		model->append(name, action);
	}
	modelStack.push(model);
	menu->doSubmenu(model);
	songs->deleteAll();
	delete songs;
}

void MPDC::update()
{
	mpd.update();
	Application::update();
}

void MPDC::event(Event &e)
{
	switch (e.type) {
	case KEY_DOWN:
		switch (e.key.value) {
		case KEY_MENU:
			if (playerActive) {
				removeWidget(player);
				delete player;
				activate(menu);
				playerActive = false;
			}
			else if (modelStack.count() == 1)
				unload();
			else {
				menu->up();
				delete modelStack.pop();
			}
			return;
		default:
			Application::event(e);
			break;
		}
		break;
	case BUTTON_DOWN:
		switch (e.button.button) {
		case BUTTON_MIDDLE:
			if (playerActive) {
				removeWidget(player);
				delete player;
				activate(menu);
				playerActive = false;
			}
			else if (modelStack.count() == 1)
				unload();
			else {
				menu->up();
				delete modelStack.pop();
			}
			return;
		default:
			Application::event(e);
			break;
		}
		break;
	default:
		Application::event(e);
		break;
	}
}

APPLICATION(MPDC)
