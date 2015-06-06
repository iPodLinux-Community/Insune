#include <stdlib.h>
#include <unistd.h>

#include "engine.h"
#include "exception.h"

void update(hd_engine *eng, int x, int y, int w, int h)
{
#ifndef SDL
	HD_LCD_Update(eng->screen.framebuffer, 0, 0,
			eng->screen.width, eng->screen.height);
#else
	SDL_UpdateRect(SDL_GetVideoSurface(), x, y, w, h);
#endif
}

#ifdef SDL
Engine::Engine(int _w, int _h)
 : w(_w), h(_h), disposed(false)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw Exception(SDL_GetError());
	if (!(screen = SDL_SetVideoMode(w, h, 16, SDL_SWSURFACE)))
		throw Exception(SDL_GetError());
	eng = HD_Initialize(w, h, 16, screen->pixels, update);
#else
Engine::Engine()
 : disposed(false)
{
	HD_LCD_Init();
	HD_LCD_GetInfo(0, &w, &h, 0);
	screen = (uint *)xmalloc(w * h * 2);
	eng = HD_Initialize(w, h, 16, screen, update);
#endif
}

void Engine::dispose()
{
	if (disposed) return;
	HD_Deinitialize(eng);
#ifndef SDL
	HD_LCD_Quit();
#else
	SDL_Quit();
#endif
	disposed = true;
}

Engine::~Engine()
{
	if (disposed) return;
	HD_Deinitialize(eng);
#ifndef SDL
	HD_LCD_Quit();
#else
	SDL_Quit();
#endif
	disposed = true;
}

void Engine::render()
{
#ifdef SDL
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);
#endif
	HD_Render(eng);
#ifdef SDL
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
#endif
}

void Engine::animate()
{
	HD_Animate(eng);
}

int Engine::width() const
{
	return w;
}

int Engine::height() const
{
	return h;
}

#ifdef SDL
static bool event_sdl2ev(SDL_Event *oe, Event *ne)
{
	switch (oe->type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		ne->type = (oe->type == SDL_KEYUP) ? KEY_UP : KEY_DOWN;
		switch (oe->key.keysym.sym) {
		case SDLK_w:
		case SDLK_LEFT:
			ne->key.value = KEY_REWIND;
			break;
		case SDLK_f:
		case SDLK_RIGHT:
			ne->key.value = KEY_FFORWARD;
			break;
		case SDLK_d:
		case SDLK_DOWN:
			ne->key.value = KEY_PLAY;
			break;
		case SDLK_m:
		case SDLK_UP:
			ne->key.value = KEY_MENU;
			break;
		case SDLK_RETURN:
			ne->key.value = KEY_ACTION;
			break;
		case SDLK_l:
		case SDLK_e:  /* makes desktop scrolling easier */
			ne->key.value = SCROLL_LEFT;
			break;
		case SDLK_r:
			ne->key.value = SCROLL_RIGHT;
			break;
		default:
			return 0;
			break;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		ne->type = (oe->type == SDL_MOUSEBUTTONUP) ? BUTTON_UP : BUTTON_DOWN;
		
		ne->button.x = oe->button.x;
		ne->button.y = oe->button.y;
		switch (oe->button.button) {
		case SDL_BUTTON_LEFT:
			ne->button.button = BUTTON_LEFT;
			break;
		case SDL_BUTTON_MIDDLE:
			ne->button.button = BUTTON_MIDDLE;
			break;
		case SDL_BUTTON_RIGHT:
			ne->button.button = BUTTON_RIGHT;
			break;
		case SDL_BUTTON_WHEELUP:
			ne->button.button = BUTTON_WHEELUP;
			break;
		case SDL_BUTTON_WHEELDOWN:
			ne->button.button = BUTTON_WHEELDOWN;
			break;
		default:
			return 0;
			break;
		}
		break;
	case SDL_MOUSEMOTION:
		ne->type = MOTION;
		ne->motion.x = oe->motion.x;
		ne->motion.y = oe->motion.y;
		break;
	case SDL_QUIT:
		exit(0);
	default: return 0;
	}
	return 1;
}
#else
static bool event_raw2ev(char oe, Event *ne)
{
	ne->type = KEY_DOWN;
	switch (oe) {
	case 'w':
		ne->key.value = KEY_REWIND;
		break;
	case 'f':
		ne->key.value = KEY_FFORWARD;
		break;
	case 'd':
		ne->key.value = KEY_PLAY;
		break;
	case 'm':
		ne->key.value = KEY_MENU;
		break;
	case '\r':
		ne->key.value = KEY_ACTION;
		break;
	case 'l':
		ne->key.value = SCROLL_LEFT;
		break;
	case 'r':
		ne->key.value = SCROLL_RIGHT;
		break;
	default: return 0;
	}
	return 1;
}
#endif

bool Engine::pollEvent(Event *e)
{
	int isevent = 0;
#ifdef SDL
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		isevent += event_sdl2ev(&event, e);
	}
#else
	char ch;
	fd_set rd;
	struct timeval tv;
	int n;

	FD_ZERO(&rd);
	FD_SET(0, &rd);

	tv.tv_sec = 0;
	tv.tv_usec = 100;

	n = select(0 + 1, &rd, NULL, NULL, &tv);
	if (FD_ISSET(0, &rd) && (n > 0)) {
		read(0, &ch, 1);
		isevent += event_raw2ev(ch, e);
	}
#endif
	return !!isevent;
}

void Engine::waitEvent(Event *e)
{
#ifdef SDL
	SDL_Event event;
	do {
		SDL_WaitEvent(&event);
	} while (!event_sdl2ev(&event, e));
#else
	char ch;
	do {
		read(0, &ch, 1);
	} while (!event_raw2ev(ch, e));
#endif
}

void Engine::add(hd_object *o)
{
	HD_Register(eng, o);
}

void Engine::remove(hd_object *o)
{
	HD_Deregister(eng, o);
}
