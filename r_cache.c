#include "base.h"
#include "panic.h"
#include "memory.h"
#include "r_cache.h"



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
