#include "base.h"
#include "config.h"
#include "r_main.h"
#include "panic.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// TODO: Move this into r_surf or something, doesn't belong here.
/*
static SDL_Surface *LoadSurface(const char *path)
{
	SDL_Surface *s = NULL, *opt = NULL;
	//FileHandle hnd = Files_OpenFile(path);

	s = IMG_Load(path);//Files_GetPath();
	if (!s) {
		Panic(Fmt("Failed to load %s", path));
	}

	opt = SDL_ConvertSurface(s, s_Rend.wSurface->format, NULL);
	SDL_FreeSurface(s);
	if (!opt) {
		Panic(Fmt("Failed to optimise %s", path));
	}

	return opt;
}
*/
typedef struct RInfo_s {
	SDL_Window *window;
	SDL_Surface *wSurface;
} RInfo;

static RInfo s_Rend;

/*
 * Rend_Init
 */
ecode_t Rend_Init()
{
	if (s_Rend.window != NULL) {
		Panic("Window already created");
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Panic("SDL_Init() failed");
	}

	if (IMG_Init(IMG_INIT_PNG) < 0) {
		Panic("IMG_Init() failed");
	}

	s_Rend.window = SDL_CreateWindow(g_Config.gameName,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				g_Config.windowWidth,
				g_Config.windowHeight,
				SDL_WINDOW_SHOWN);
	if (!s_Rend.window)
		Panic("Failed to create window");

	s_Rend.wSurface = SDL_GetWindowSurface(s_Rend.window);
	SDL_FillRect(s_Rend.wSurface, NULL, SDL_MapRGB(s_Rend.wSurface->format,
						0, 0, 0));
	SDL_UpdateWindowSurface(s_Rend.window);

	Trace(CHAN_REND, Fmt("initialised renderer %dx%d", g_Config.windowWidth,
						g_Config.windowHeight));

	return EOK;
}

/*
 * Rend_Shutdown
 */
ecode_t Rend_Shutdown()
{
	if (s_Rend.window != NULL) {
		IMG_Quit();
		SDL_DestroyWindow(s_Rend.window);
		SDL_Quit();

		s_Rend.window = NULL;
		s_Rend.wSurface = NULL;

		Trace(CHAN_REND, "shutdown");

		return EOK;
	}

	Trace(CHAN_REND, "called unnecessarily");

	return EFAIL;
}

/*
 * Rend_Frame
 *	Render the current frame.
 */
ecode_t Rend_Frame()
{
	if (!s_Rend.window) {
		Panic("Renderer not initialised");
	}

	SDL_UpdateWindowSurface(s_Rend.window);

	return EOK;
}

/*
 * Rend_GetWindow
 */
SDL_Window *Rend_GetWindow()
{
	if (!s_Rend.window) {
		Panic("Renderer not initialised");
	}

	return s_Rend.window;
}
