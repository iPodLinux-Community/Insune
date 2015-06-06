#ifndef _ENGINE_H_
#define _ENGINE_H_

#ifdef SDL
#include <SDL.h>
#else
#include "terminal.h"
#endif

#include "hotdog.h"

typedef unsigned int uint;

enum KeyType {
	KEY_REWIND   = 'w',
	KEY_FFORWARD = 'f',
	KEY_PLAY     = 'd',
	KEY_MENU     = 'm',
	KEY_ACTION   = '\r',
	SCROLL_LEFT  = 'l',
	SCROLL_RIGHT = 'r'
};

enum ButtonType {
	BUTTON_LEFT       = '1',
	BUTTON_MIDDLE     = '2',
	BUTTON_RIGHT      = '3',
	BUTTON_WHEELUP    = '4',
	BUTTON_WHEELDOWN  = '5'
};

enum EventType {
	KEY_UP      = 1     ,
	KEY_DOWN    = 1 << 1,
	BUTTON_UP   = 1 << 2,
	BUTTON_DOWN = 1 << 3,
	MOTION      = 1 << 4
};

typedef struct _Event {
	EventType type;
	union {
		struct {
			int x, y;
		} motion;
		struct {
			int x, y;
			ButtonType button;
		} button;
		struct {
			KeyType value;
		} key;
	};
} Event;

#ifndef SDL
#define SCROLL_MOD(n) \
  do { \
    static int sofar = 0; \
    if (++sofar < n) \
      return; \
    sofar -= n; \
  } while (0)
#else
#define SCROLL_MOD(n)
#endif


class Engine
{
 public:
#ifdef SDL
	Engine(int w = 220, int h = 176);
#else
	Engine();
#endif
	~Engine();

	void render();
	void animate();
	bool pollEvent(Event *);
	void waitEvent(Event *);

	void add(hd_object *);
	void remove(hd_object *);
	int width() const;
	int height() const;

	void dispose();

 private:
 	int w, h;
	hd_engine *eng;
	bool disposed;
#ifndef SDL
	Terminal term;

	uint *screen;
#else
	SDL_Surface *screen;
#endif
};

#endif
