#include "base.h"
#include "config.h"
#include "r_main.h"
#include "panic.h"
#include "list.h"
#include "memory.h"
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
// TODO: Multiple queues for each type of command
static LIST_HEAD(command_list);

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

/*
 * load_fonts
 */
static void load_fonts()
{
        s_state.small_font = TTF_OpenFont(FONT_FILENAME, FONT_SMALLSIZE);
        if (!s_state.small_font) {
                Panic(Fmt("Failed: %s", TTF_GetError()));
        }

        s_state.normal_font = TTF_OpenFont(FONT_FILENAME, FONT_NORMSIZE);
        if (!s_state.normal_font) {
                Panic(Fmt("Failed: %s", TTF_GetError()));
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
        } else if (sz == FONT_NORMAL) {
                surf = TTF_RenderText_Solid(s_state.normal_font, str, col);
        }

        ret.texture = SDL_CreateTextureFromSurface(s_state.renderer, surf);
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
        SDL_RenderCopy(s_state.renderer, rstr->texture, NULL, &dst);
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
static void queue_command(struct render_command *cmd)
{
        list_add(&cmd->list, &command_list);
}

/*
 * destroy_command
 *      Free all the memory the given command used.
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
                Panic("Render command added outside of entity render function");
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

        queue_command(cmd);
}

/*
 * R_AddCircle
 */
void R_AddCircle(Colour c, int x, int y, float r)
{
        check_accepting();
        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_CIRCLE;
        cmd->shape.colour = c;
        cmd->shape.x = x;
        cmd->shape.y = y;
        cmd->shape.r = r;

        queue_command(cmd);
}

/*
 * R_AddLine
 */
void R_AddLine(Colour c, int sx, int sy, int ex, int ey)
{
        check_accepting();
        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_LINE;
        cmd->shape.colour = c;
        cmd->shape.x = sx;
        cmd->shape.y = sy;
        cmd->shape.x2 = ex;
        cmd->shape.y2 = ey;

        queue_command(cmd);
}

/*
 * R_AddRect
 */
void R_AddRect(Colour c, int x, int y, int w, int h)
{
        check_accepting();
        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_RECT;
        cmd->shape.colour = c;
        cmd->shape.x = x;
        cmd->shape.y = y;
        cmd->shape.w = w;
        cmd->shape.h = h;

        queue_command(cmd);
}

/*
 * R_AddPoint
 */
void R_AddPoint(Colour c, int x, int y)
{
        check_accepting();
        struct render_command *cmd = create_command();

        cmd->type = RC_SHAPE;
        cmd->shape.type = SHAPE_POINT;
        cmd->shape.colour = c;
        cmd->shape.x = x;
        cmd->shape.y = y;

        queue_command(cmd);
}

/*
 * Rend_Init
 */
ecode_t Rend_Init()
{
	if (s_state.window != NULL) {
		Panic("Window already created");
	}

        rcmd_pool = LAlloc_Create(RCMD_POOL_SZ * sizeof(struct render_command),
                "rcmds");

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

        uint32_t flags = SDL_RENDERER_ACCELERATED;

        if (g_Config.vsync)
                flags |= SDL_RENDERER_PRESENTVSYNC;

        s_state.renderer = SDL_CreateRenderer(s_state.window, -1, flags);
        if (!s_state.renderer) {
                Panic("Failed to create renderer");
        }

        set_colour(COLOUR_BLACK);
        load_fonts();

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
                LAlloc_Destroy(rcmd_pool);

                SDL_DestroyRenderer(s_state.renderer);
		SDL_DestroyWindow(s_state.window);
                TTF_CloseFont(s_state.normal_font);
                TTF_CloseFont(s_state.small_font);

                TTF_Quit();
                IMG_Quit();
		SDL_Quit();

                memset(&s_state, 0, sizeof(s_state));

		Trace(CHAN_REND, "shutdown");

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

/*
 * process_commands
 *      Process all commands in the command queue.
 */
static void process_commands()
{
        struct list_head *iter, *safe;
        list_for_each_safe(iter, safe, &command_list) {
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
                LAlloc_Reset(rcmd_pool);
        }
}

static uint32_t rcmd_count()
{
        struct render_command *cmd = NULL;
        uint32_t n = 0;

        list_for_each_entry(cmd, &command_list, list) {
                n++;
        }

        return n;
}

/*
 * Rend_Frame
 *	Render the current frame.
 */
ecode_t Rend_Frame()
{
	if (!s_state.window) {
		Panic("Renderer not initialised");
	}

        uint8_t r, g, b, a;

        SDL_RenderClear(s_state.renderer);
        SDL_GetRenderDrawColor(s_state.renderer, &r, &g, &b, &a);

        /* Output the number of commands, this can be removed. */
        accepting_cmds = true;
        R_AddString(FONT_NORMAL, COLOUR_WHITE, 10, g_Config.windowHeight - 50,
                Fmt("rcmds: %u", rcmd_count()));
        accepting_cmds = false;

        process_commands();

        SDL_SetRenderDrawColor(s_state.renderer, r, g, b, a);
        SDL_RenderPresent(s_state.renderer);

	return EOK;
}
