#include "base.h"
#include "globals.h"
#include "renderer.h"
#include "panic.h"
#include <SDL2/SDL.h>

/*
 * 
 */
typedef struct RInfo_s {
	SDL_Window *window;
	SDL_Surface *wSurface;
} RInfo;

static RInfo s_Rend;

int RendererInit()
{
	if (s_Rend.window != NULL) {
		Panic("RendererInit(): window already created");
		return 1;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Panic("RendererInit(): SDL_Init() failed");
		return 1;
	}

	s_Rend.window = SDL_CreateWindow(g_Globs.windowTitle,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				g_Globs.windowWidth,
				g_Globs.windowHeight,
				SDL_WINDOW_SHOWN);
	if (!s_Rend.window) {
		Panic("RendererInit(): failed to create window");
		return 1;
	}

	s_Rend.wSurface = SDL_GetWindowSurface(s_Rend.window);
	SDL_FillRect(s_Rend.wSurface, NULL, SDL_MapRGB(s_Rend.wSurface->format,
						0, 0, 0));
	SDL_UpdateWindowSurface(s_Rend.window);

	return 0;
}

int RendererShutdown()
{
	if (s_Rend.window != NULL) {
		SDL_DestroyWindow(s_Rend.window);
		SDL_Quit();

		s_Rend.window = NULL;
		s_Rend.wSurface = NULL;

		return 0;
	}

	return 1;
}

int RendererFrame()
{
	if (!s_Rend.window) {
		Panic("RendererFrame(): renderer not initialised");
		return 1;
	}

	SDL_UpdateWindowSurface(s_Rend.window);

	return 0;
}
