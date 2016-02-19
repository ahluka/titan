#include "base.h"
#include "panic.h"
#include "memory.h"
#include "r_cache.h"


// SDL_DestroyTexture
// static SDL_Texture *load_texture(const char *filename)
// {
//         assert(filename != NULL);
//
//         SDL_Texture *ret = NULL;
//         SDL_Surface *surf = NULL;
//
//         surf = IMG_Load(filename);
//         if (!surf) {
//                 Panic(Fmt("Failed to load %s", filename));
//         }
//
//         ret = SDL_CreateTextureFromSurface(s_state.renderer, surf);
//         if (!ret) {
//                 Panic(Fmt("Failed to create texture from %s (%s)", filename,
//                         SDL_GetError()));
//         }
//
//         SDL_FreeSurface(surf);
//         return ret;
// }
