/*
 * gameloop
 *	The main loop and gamestate updating.
 */
#include "base.h"
#include "r_main.h"
#include "timer.h"
#include "memory.h"
#include "globals.h"
#include "panic.h"
#include "entity.h"
#include "input.h"
#include "event.h"
#include "config.h"
#include <SDL2/SDL.h>


UNUSED static ecode_t test_update(entity_t *self, float dT)
{
        VMA(self->pos, self->vel, dT);

        return EOK;
}

UNUSED static ecode_t test_render(entity_t *self)
{
        r_add_rect(COLOUR_RED, self->pos[X], self->pos[Y], 35, 35);

        return EOK;
}

/*
 * check_dbg_keys
 *	Checks if a debug key was pressed and runs commands as necessary.
 */
static void check_dbg_keys(SDL_Keycode code)
{
	switch (code) {
	case SDLK_F12:
		MemStats();
		break;
	}
}

/*
 * UpdateGameworld
 *	Update the gameworld state.
 */
static void update_gameworld(float dT)
{
	if (update_entities(dT) != EOK) {
		panic("Failed to update Entities");
	}

	if (process_events() != EOK) {
		panic("Failed to process events");
	}
}


/*
 * mainloop
 * Enter the main loop. Each iteration:
 *	- Read user input
 *	- Update gameworld
 *	- Render view
 */
ecode_t mainloop()
{
	SDL_Event event;
	bool quit = false;
	struct timer gameTimer;
	struct timer stepTimer;
	uint32_t frameCount = 0, nextFPS = 0, fps = 0;

	if (init_renderer() != EOK) {
		trace(CHAN_INFO, "failed to init renderer");
		return EFAIL;
	}

	if (init_entities() != EOK) {
		trace(CHAN_INFO, "failed to init entity manager");
		return EFAIL;
	}

        // Entity *test = Ent_Spawn("default");
        // test->Render = TestRender;
        // test->Update = TestUpdate;
        // test->updateType = ENT_UPDATE_FRAME;
        // test->nextUpdate = g_globals.timeNowMs + 1000;
        // Ent_Free(test);

        start_timer(&gameTimer);

	while (!quit) {
		/* input */
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			} else if (event.type == SDL_KEYUP) {
				In_SetKeyUp((int32_t) event.key.keysym.scancode);
			} else if (event.type == SDL_KEYDOWN) {
				check_dbg_keys(event.key.keysym.sym);
				In_SetKeyDown((int32_t) event.key.keysym.scancode);

				// FIXME: this is for debugging only; remove it
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}
			}
		}

		/* Update gameworld */
		float dT = timer_get_ticks(&stepTimer) / 1000.f;
		g_globals.timeNowMs = timer_get_ticks(&gameTimer);
		update_gameworld(dT);
		start_timer(&stepTimer); /* restart */

                /* FPS */
                if (nextFPS <= g_globals.timeNowMs) {
                        nextFPS = g_globals.timeNowMs + 1000;
                        fps = frameCount;
                        frameCount = 0;
                }

		/* Render */
                r_begin_commands();
                r_add_string(FONT_NORMAL, COLOUR_WHITE, 10, 10,
                        fmt("FPS: %u - dT: %3.4f - T: %u - M: %lu bytes", fps, dT,
                        g_globals.timeNowMs, MemCurrentUsage()));

                if (render_all_entities() != EOK) {
                        panic("Failed to render entities");
                }
                r_end_commands();

		r_render_frame();
		frameCount++;
	}

	if (shutdown_entities() != EOK) {
		trace(CHAN_INFO, "failed to shutdown entity manager");
		return EFAIL;
	}

	if (shutdown_renderer() != EOK) {
		trace(CHAN_INFO, "failed to shutdown renderer");
		return EFAIL;
	}

	return EOK;
}
