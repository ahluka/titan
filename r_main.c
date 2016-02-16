#include "base.h"
#include "config.h"
#include "r_main.h"
#include "panic.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// TODO: Get preliminary command pipeline written--just debug text for now

#define FONT_FILENAME "./res/pragmatapro.ttf"
#define FONT_PTSIZE 16

/* The global renderer state. */
// TODO: This might need sharing between modules, unfortunately
struct rstate {
	SDL_Window *window;
        SDL_Renderer *renderer;
        TTF_Font *font;
};

/* An SDL_Texture created from the renderer font, with the original surface's
 * with and height, so it can be rendered properly. */
struct rstring {
        SDL_Texture *texture;
        int w, h;
};

static struct rstate s_state;
static struct rstring s_test = {NULL, 0, 0};

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

static void load_font()
{
        s_state.font = TTF_OpenFont(FONT_FILENAME, FONT_PTSIZE);
        if (!s_state.font) {
                Panic(Fmt("Failed: %s", TTF_GetError()));
        }
}

/*
 * create_rstring
 *      Render the given string into a surface, convert it into the texture,
 *      then return it along with the surface's width and height (an rstring).
 * TODO: Colours.
 */
static struct rstring create_rstring(const char *str)
{
        assert(str != NULL);

        SDL_Color col = {255, 255, 255};
        struct rstring ret = {NULL, 0, 0};

        SDL_Surface *surf = TTF_RenderText_Solid(s_state.font, str, col);
        ret.texture = SDL_CreateTextureFromSurface(s_state.renderer, surf);
        ret.w = surf->w;
        ret.h = surf->h;
        SDL_FreeSurface(surf);

        return ret;
}

/*
 * render_rstring
 *      Render the given struct rstring at the specified coords.
 */
static void render_rstring(struct rstring *rstr, int x, int y)
{
        assert(rstr != NULL);

        SDL_Rect dst = {x, y, rstr->w, rstr->h};
        SDL_RenderCopy(s_state.renderer, rstr->texture, NULL, &dst);
}

/*
 * Rend_Init
 */
ecode_t Rend_Init()
{
	if (s_state.window != NULL) {
		Panic("Window already created");
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Panic("SDL_Init() failed");
	}

	if (IMG_Init(IMG_INIT_PNG) < 0) {
		Panic("IMG_Init() failed");
	}

        if (TTF_Init() < 0) {
                Panic("TTF_Init() failed");
        }

	s_state.window = SDL_CreateWindow(g_Config.gameName,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				g_Config.windowWidth,
				g_Config.windowHeight,
				SDL_WINDOW_SHOWN);
	if (!s_state.window)
		Panic("Failed to create window");

        s_state.renderer = SDL_CreateRenderer(s_state.window, -1,
                SDL_RENDERER_ACCELERATED);
        if (!s_state.renderer) {
                Panic("Failed to create renderer");
        }

        SDL_SetRenderDrawColor(s_state.renderer, 48, 48, 48, 255);

        load_font();
        s_test = create_rstring("shit son");

	Trace(CHAN_REND, Fmt("initialised renderer %dx%d", g_Config.windowWidth,
						g_Config.windowHeight));

	return EOK;
}

/*
 * Rend_Shutdown
 */
ecode_t Rend_Shutdown()
{
	if (s_state.window != NULL) {
                SDL_DestroyRenderer(s_state.renderer);
		SDL_DestroyWindow(s_state.window);
                TTF_CloseFont(s_state.font);
                TTF_Quit();
                IMG_Quit();
		SDL_Quit();

                s_state.renderer = NULL;
		s_state.window = NULL;

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
//SDL_RenderCopy(s_state.renderer, texture, NULL, NULL);
// SDL_RenderCopy()'s last two args are the source and dest SDL_Rects.
ecode_t Rend_Frame()
{
	if (!s_state.window) {
		Panic("Renderer not initialised");
	}

        uint8_t r, g, b, a;

        SDL_RenderClear(s_state.renderer);
        SDL_GetRenderDrawColor(s_state.renderer, &r, &g, &b, &a);

        render_rstring(&s_test, 10, 10);

        SDL_SetRenderDrawColor(s_state.renderer, r, g, b, a);
        SDL_RenderPresent(s_state.renderer);

	return EOK;
}
