#include "base.h"
#include "globals.h"
#include "renderer.h"
#include "panic.h"
#include <SDL2/SDL.h>


typedef struct RInfo_s {
	SDL_Window *window;
	SDL_Surface *wSurface;
} RInfo;

static RInfo s_Rend;

ecode_t Rend_Init()
{
	if (s_Rend.window != NULL)
		Panic("Window already created");

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		Panic("SDL_Init() failed");

	s_Rend.window = SDL_CreateWindow(g_Globals.windowTitle,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				g_Globals.windowWidth,
				g_Globals.windowHeight,
				SDL_WINDOW_SHOWN);
	if (!s_Rend.window)
		Panic("Failed to create window");

	s_Rend.wSurface = SDL_GetWindowSurface(s_Rend.window);
	SDL_FillRect(s_Rend.wSurface, NULL, SDL_MapRGB(s_Rend.wSurface->format,
						0, 0, 0));
	SDL_UpdateWindowSurface(s_Rend.window);

	return EOK;
}

ecode_t Rend_Shutdown()
{
	if (s_Rend.window != NULL) {
		SDL_DestroyWindow(s_Rend.window);
		SDL_Quit();

		s_Rend.window = NULL;
		s_Rend.wSurface = NULL;

		return EOK;
	}

	Trace("RendererShutdown called unnecessarily");

	return EFAIL;
}

ecode_t Rend_Frame()
{
	if (!s_Rend.window)
		Panic("Renderer not initialised");

	SDL_UpdateWindowSurface(s_Rend.window);

	return EOK;
}

SDL_Window *Rend_GetWindow()
{
	if (!s_Rend.window)
		Panic("Renderer not initialised");

	return s_Rend.window;
}
