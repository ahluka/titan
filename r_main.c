#include "base.h"
#include "config.h"
#include "r_main.h"
#include "panic.h"
#include "list.h"
#include "memory.h"
#include "vec.h"
#include "r_viewport.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define FONT_FILENAME "./res/pragmatapro.ttf"
#define FONT_SMALLSIZE 14
#define FONT_NORMSIZE 16

/* We use a linear allocator to hold the render commands for each frame,
 * so a maximum of RCMD_POOL_SZ can exist at once. */
#define RCMD_POOL_SZ 32

/* The global renderer state. */
// TODO: This might need sharing between modules, unfortunately
struct rstate {
	SDL_Window *window;
        SDL_Renderer *renderer;
        struct viewport viewport;
        TTF_Font *small_font, *normal_font;
};

/* An SDL_Texture created from the renderer font, with the original surface's
 * width and height, so it can be rendered properly. */
struct rstring {
        SDL_Texture *texture;
        int w, h;
};

/* Render commands. Each render_command is a tagged union and they're queued
 * up ready for processing at the end of every frame. A separate queue is
 * maintained for each type of render command so they can be processed in the
 * ideal order--this is simpler than sorting a single list every frame.
 *
 * Each queue *could* be a list of different structs, but it isn't so that a
 * single memory pool can be used.
 */
enum command_type {
        RC_TEXT,
        RC_SHAPE,
        RC_SPRITE,
        RC_PSYSTEM
};

enum shape_type {
        SHAPE_CIRCLE,   // TODO: implement this
        SHAPE_LINE,
        SHAPE_RECT,
        SHAPE_POINT
};

struct text_cmd {
        FontSize size;
        Colour colour;
        char *str;
        int x, y;
};

struct shape_cmd {
        enum shape_type type;
        Colour colour;
        int x, y, x2, y2, w, h;
        float r;
};

struct sprite_cmd {
        int x, y;
};

struct psystem_cmd {
        int x, y;
};

struct render_command {
        struct list_head list;
        enum command_type type;

        union {
                struct text_cmd text;
                struct shape_cmd shape;
                struct sprite_cmd sprite;
                struct psystem_cmd psystem;
        };
};

/* The queue of render_command's. */
static uint32_t discarded_cmds = 0;
static LIST_HEAD(text_rcmds_list);
static LIST_HEAD(shape_rcmds_list);
static LIST_HEAD(sprite_rcmds_list);
static LIST_HEAD(psys_rcmds_list);

static LAllocState *rcmd_pool = NULL;

/* Static renderer state. */
static struct rstate s_state;
static bool accepting_cmds = false;

/* These map enum Colour values to SDL_Color structures. */
static SDL_Color colour_table[] = {
        {0, 0, 0, 255},
        {255, 255, 255, 255},
        {255, 0, 0, 255},
        {0, 255, 0, 255},
        {0, 0, 255, 255},
        {48, 48, 48, 255},
        {255, 0, 255, 255}
};

static void set_colour(enum Colour colour)
{
        SDL_Color c = colour_table[colour];
        SDL_SetRenderDrawColor(s_state.renderer, c.r, c.g, c.b, c.a);
}

/*
 * lerp_colour
 */
UNUSED static void
lerp_colour(SDL_Color *out, SDL_Color *from, SDL_Color *to, float t)
{
        out->r = lerp(from->r, to->r, t);
        out->g = lerp(from->g, to->g, t);
        out->b = lerp(from->b, to->b, t);
        out->a = 255; /* Is this what we want? */
}

/*
 * load_fonts
 */
static void load_fonts()
{
        s_state.small_font = TTF_OpenFont(FONT_FILENAME, FONT_SMALLSIZE);
        if (!s_state.small_font) {
                Panic(Fmt("Failed to load fonts: %s", TTF_GetError()));
        }

        s_state.normal_font = TTF_OpenFont(FONT_FILENAME, FONT_NORMSIZE);
        if (!s_state.normal_font) {
                Panic(Fmt("Failed to load fonts: %s", TTF_GetError()));
        }
}

/*
 * create_rstring
 *      Render the given string into a surface, convert it into a texture,
 *      then return it along with the surface's width and height (an rstring).
 */
static struct rstring
create_rstring(Colour colour, FontSize sz, const char *str)
{
        assert(str != NULL);

        SDL_Color col = colour_table[colour];
        struct rstring ret = {NULL, 0, 0};

        SDL_Surface *surf = NULL;
        if (sz == FONT_SMALL) {
                surf = TTF_RenderText_Solid(s_state.small_font, str, col);
                if (!surf) {
                        Panic(Fmt("Failed to render string (%s)", TTF_GetError()));
                }
        } else if (sz == FONT_NORMAL) {
                surf = TTF_RenderText_Solid(s_state.normal_font, str, col);
                if (!surf) {
                        Panic(Fmt("Failed to render string (%s)", TTF_GetError()));
                }
        }

        ret.texture = SDL_CreateTextureFromSurface(s_state.renderer, surf);
        if (!ret.texture) {
                Panic(Fmt("Failed to create texture (%s)", SDL_GetError()));
        }

        ret.w = surf->w;
        ret.h = surf->h;
        SDL_FreeSurface(surf);

        return ret;
}

/*
 * render_rstring
 *      Render the given rstring at the specified coords.
 */
static void render_rstring(struct rstring *rstr, int x, int y)
{
        assert(rstr != NULL);

        SDL_Rect dst = {x, y, rstr->w, rstr->h};
        if (SDL_RenderCopy(s_state.renderer, rstr->texture, NULL, &dst) < 0) {
                Panic(Fmt("SDL_RenderCopy failed (%s)", SDL_GetError()));
        }
}

/*
 * create_command
 */
static struct render_command *create_command()
{
        return (struct render_command *) LAlloc(rcmd_pool,
                sizeof(struct render_command));
}

/*
 * queue_command
 *      Add a render_command to the queue.
 */
static void queue_command(struct render_command *cmd, struct list_head *q)
{
        list_add(&cmd->list, q);
}

/*
 * destroy_command
 *      Free all the memory the given command used, but not the command
 *      itself.
 */
static void destroy_command(struct render_command *cmd)
{
        switch (cmd->type) {
        case RC_TEXT:
                sstrfree(cmd->text.str);
                break;
        case RC_SHAPE:
                break;
        case RC_SPRITE:
                break;
        case RC_PSYSTEM:
                break;
        }
}

/*
 * check_accepting
 *      Check if we're currrently accepting render commands. If we aren't,
 *      Panic().
 */
static void check_accepting()
{
        if (!accepting_cmds) {
                Panic("Command submitted outside of entity render function");
        }
}

/*
 * R_AddString
 */
void R_AddString(FontSize sz, Colour c, int x, int y, const char *str)
{
        check_accepting();
        struct render_command *cmd = create_command();

        cmd->type = RC_TEXT;
        cmd->text.size = sz;
        cmd->text.colour = c;
        cmd->text.str = sstrdup(str);
        cmd->text.x = x;
        cmd->text.y = y;

        queue_command(cmd, &text_rcmds_list);
}

/*
 * R_AddCircle
 */
void R_AddCircle(Colour c, int x, int y, float r)
{
        check_accepting();

        if (!viewport_contains_xy(&s_state.viewport, x, y)) {
                discarded_cmds++;
                return;
        }

        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_CIRCLE;
        cmd->shape.colour = c;
        cmd->shape.x = x;
        cmd->shape.y = y;
        cmd->shape.r = r;

        queue_command(cmd, &shape_rcmds_list);
}

/*
 * R_AddLine
 */
void R_AddLine(Colour c, int sx, int sy, int ex, int ey)
{
        check_accepting();

        if (!viewport_contains_xy(&s_state.viewport, sx, sy) &&
                !viewport_contains_xy(&s_state.viewport, ex, ey)) {
                discarded_cmds++;
                return;
        }

        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_LINE;
        cmd->shape.colour = c;
        cmd->shape.x = sx;
        cmd->shape.y = sy;
        cmd->shape.x2 = ex;
        cmd->shape.y2 = ey;

        queue_command(cmd, &shape_rcmds_list);
}

/*
 * R_AddRect
 */
void R_AddRect(Colour c, int x, int y, int w, int h)
{
        check_accepting();

        if (!viewport_contains_xy(&s_state.viewport, x, y)) {
                discarded_cmds++;
                return;
        }

        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_RECT;
        cmd->shape.colour = c;
        cmd->shape.x = x;
        cmd->shape.y = y;
        cmd->shape.w = w;
        cmd->shape.h = h;

        queue_command(cmd, &shape_rcmds_list);
}

/*
 * R_AddPoint
 */
void R_AddPoint(Colour c, int x, int y)
{
        check_accepting();

        if (!viewport_contains_xy(&s_state.viewport, x, y)) {
                discarded_cmds++;
                return;
        }

        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_POINT;
        cmd->shape.colour = c;
        cmd->shape.x = x;
        cmd->shape.y = y;

        queue_command(cmd, &shape_rcmds_list);
}

/*
 * R_Init
 */
ecode_t R_Init()
{
	if (s_state.window != NULL) {
		Panic("Renderer already initialised");
	}

        rcmd_pool = LAlloc_Create(RCMD_POOL_SZ * sizeof(struct render_command),
                "rcmds");

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Panic(Fmt("SDL_Init() failed (%s)", SDL_GetError()));
	}

	if (IMG_Init(IMG_INIT_PNG) < 0) {
		Panic(Fmt("IMG_Init() failed (%s)", IMG_GetError()));
	}

        if (TTF_Init() < 0) {
                Panic(Fmt("TTF_Init() failed (%s)", TTF_GetError()));
        }

        atexit(TTF_Quit);
        atexit(IMG_Quit);
        atexit(SDL_Quit);

	s_state.window = SDL_CreateWindow(g_Config.gameName,
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				g_Config.windowWidth,
				g_Config.windowHeight,
				SDL_WINDOW_SHOWN);
	if (!s_state.window)
		Panic(Fmt("Failed to create window (%s)", SDL_GetError()));

        uint32_t flags = SDL_RENDERER_ACCELERATED;

        if (g_Config.vsync)
                flags |= SDL_RENDERER_PRESENTVSYNC;

        s_state.renderer = SDL_CreateRenderer(s_state.window, -1, flags);
        if (!s_state.renderer)
                Panic(Fmt("Failed to create renderer", SDL_GetError()));

        set_colour(COLOUR_BLACK);
        load_fonts();

        s_state.viewport = create_viewport(0, 0, g_Config.windowWidth,
                g_Config.windowHeight);

	Trace(CHAN_REND, Fmt("initialised renderer %dx%d", g_Config.windowWidth,
						g_Config.windowHeight));

	return EOK;
}

/*
 * R_Shutdown
 */
ecode_t R_Shutdown()
{
	if (s_state.window != NULL) {
                LAlloc_Destroy(rcmd_pool);

                SDL_DestroyRenderer(s_state.renderer);
		SDL_DestroyWindow(s_state.window);
                TTF_CloseFont(s_state.normal_font);
                TTF_CloseFont(s_state.small_font);

                TTF_Quit();
                IMG_Quit();
		SDL_Quit();

                memset(&s_state, 0, sizeof(s_state));

		Trace(CHAN_REND, "shutdown complete");

		return EOK;
	}

	Trace(CHAN_REND, "called unnecessarily");

	return EFAIL;
}

/*
 * R_BeginCommands
 */
void R_BeginCommands()
{
        accepting_cmds = true;
}

/*
 * R_EndCommands
 */
void R_EndCommands()
{
        accepting_cmds = false;
}

/* These are the individual command processing functions. Each one processes
 * a single render command, issuing render calls as appropriate.
 */
static void process_text_cmd(struct text_cmd *cmd)
{
        struct rstring rstr = create_rstring(cmd->colour, cmd->size, cmd->str);
        render_rstring(&rstr, cmd->x, cmd->y);
        SDL_DestroyTexture(rstr.texture);
}

static void process_shape_cmd(struct shape_cmd *cmd)
{
        switch (cmd->type) {
        case SHAPE_CIRCLE:
                Panic("Circle not impemented yet");
                break;
        case SHAPE_LINE:
                set_colour(cmd->colour);
                SDL_RenderDrawLine(s_state.renderer, cmd->x, cmd->y,
                        cmd->x2, cmd->y2);
                break;
        case SHAPE_RECT: {
                SDL_Rect r = {cmd->x, cmd->y, cmd->w, cmd->h};
                set_colour(cmd->colour);
                SDL_RenderDrawRect(s_state.renderer, &r);
        } break;
        case SHAPE_POINT:
                set_colour(cmd->colour);
                SDL_RenderDrawPoint(s_state.renderer, cmd->x, cmd->y);
                break;
        }
}

static void process_sprite_cmd(struct sprite_cmd *cmd)
{

}

static void process_psys_cmd(struct psystem_cmd *cmd)
{

}

static void process_queue(struct list_head *q)
{
        struct list_head *iter, *safe;
        list_for_each_safe(iter, safe, q) {
                struct render_command *cmd = list_entry(iter,
                        struct render_command, list);

                switch (cmd->type) {
                case RC_TEXT:
                        process_text_cmd(&cmd->text);
                        break;
                case RC_SHAPE:
                        process_shape_cmd(&cmd->shape);
                        break;
                case RC_SPRITE:
                        process_sprite_cmd(&cmd->sprite);
                        break;
                case RC_PSYSTEM:
                        process_psys_cmd(&cmd->psystem);
                        break;
                }

                list_del(iter);
                destroy_command(cmd);
        }
}

/*
 * process_commands
 *      Process all commands in the command queues. They are processed in a
 *      certain order--a good ol' painters' algorithm.
 */
static void process_commands()
{
        /* The order, from back to front (for now):
         *      map
         *      sprites
         *      psystems
         *      debug shapes
         *      debug text
         */

        // map
        // process_queue(&sprite_rcmds_list);
        // process_queue(&psys_rcmds_list);
        process_queue(&shape_rcmds_list);
        process_queue(&text_rcmds_list);

        LAlloc_Reset(rcmd_pool);
}

static void debug_commands()
{
        struct render_command *iter = NULL;
        uint32_t counts[4] = {0};
        uint32_t total = 0;

        list_for_each_entry(iter, &text_rcmds_list, list) {
                counts[RC_TEXT]++;
                total++;
        }

        list_for_each_entry(iter, &shape_rcmds_list, list) {
                counts[RC_SHAPE]++;
                total++;
        }

        list_for_each_entry(iter, &sprite_rcmds_list, list) {
                counts[RC_SPRITE]++;
                total++;
        }

        list_for_each_entry(iter, &psys_rcmds_list, list) {
                counts[RC_PSYSTEM]++;
                total++;
        }

        const char *s = Fmt("rcmds: %u (t: %u, sh: %u, sp: %u, p: %u) / %d" \
                                " - discarded: %u",
                total, counts[RC_TEXT], counts[RC_SHAPE],
                counts[RC_SPRITE], counts[RC_PSYSTEM], RCMD_POOL_SZ,
                discarded_cmds);

        accepting_cmds = true;
        R_AddString(FONT_NORMAL, COLOUR_WHITE, 10, g_Config.windowHeight - 50,
                s);
        accepting_cmds = false;
        discarded_cmds = 0;
}

/*
 * R_RenderFrame
 *	Render the current frame.
 */
ecode_t R_RenderFrame()
{
	if (!s_state.window) {
		Panic("Renderer not initialised");
	}

        uint8_t r, g, b, a;

        SDL_RenderClear(s_state.renderer);
        SDL_GetRenderDrawColor(s_state.renderer, &r, &g, &b, &a);

        debug_commands();
        process_commands();

        SDL_SetRenderDrawColor(s_state.renderer, r, g, b, a);
        SDL_RenderPresent(s_state.renderer);

	return EOK;
}
