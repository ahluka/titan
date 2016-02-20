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


UNUSED static ecode_t TestUpdate(Entity *self, float dT)
{
        VMA(self->pos, self->vel, dT);

        return EOK;
}

UNUSED static ecode_t TestRender(Entity *self)
{
        R_AddRect(COLOUR_RED, self->pos[X], self->pos[Y], 35, 35);

        return EOK;
}

/*
 * CheckDbgKeys
 *	Checks if a debug key was pressed and runs commands as necessary.
 */
static void CheckDbgKeys(SDL_Keycode code)
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
static void UpdateGameworld(float dT)
{
	if (Ent_UpdateAll(dT) != EOK) {
		Panic("Failed to update Entities");
	}

	if (Evt_Process() != EOK) {
		Panic("Failed to process events");
	}
}


/*
 * Mainloop
 * Enter the main loop. Each iteration:
 *	- Read user input
 *	- Update gameworld (run AI, etc.)
 *	- Render view
 */
ecode_t Mainloop()
{
	SDL_Event event;
	bool quit = false;
	struct Timer gameTimer;
	struct Timer stepTimer;
	uint32_t frameCount = 0, nextFPS = 0, fps = 0;

	if (R_Init() != EOK) {
		Trace(CHAN_INFO, "failed to init renderer");
		return EFAIL;
	}

	if (Ent_Init() != EOK) {
		Trace(CHAN_INFO, "failed to init entity manager");
		return EFAIL;
	}

        // Entity *test = Ent_Spawn("default");
        // test->Render = TestRender;
        // test->Update = TestUpdate;
        // test->updateType = ENT_UPDATE_FRAME;
        // test->nextUpdate = g_Globals.timeNowMs + 1000;
        // Ent_Free(test);

        Timer_Start(&gameTimer);

	while (!quit) {
		/* input */
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			} else if (event.type == SDL_KEYUP) {
				In_SetKeyUp((int32_t) event.key.keysym.scancode);
			} else if (event.type == SDL_KEYDOWN) {
				CheckDbgKeys(event.key.keysym.sym);
				In_SetKeyDown((int32_t) event.key.keysym.scancode);

				// FIXME: this is for debugging only; remove it
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
				}
			}
		}

		/* Update gameworld */
		float dT = Timer_GetTicks(&stepTimer) / 1000.f;
		g_Globals.timeNowMs = Timer_GetTicks(&gameTimer);
		UpdateGameworld(dT);
		Timer_Start(&stepTimer); /* restart */

                /* FPS */
                if (nextFPS <= g_Globals.timeNowMs) {
                        nextFPS = g_Globals.timeNowMs + 1000;
                        fps = frameCount;
                        frameCount = 0;
                }

		/* Render */
                R_BeginCommands();
                R_AddString(FONT_NORMAL, COLOUR_WHITE, 10, 10,
                        Fmt("FPS: %u - dT: %3.4f - T: %u - M: %lu bytes", fps, dT,
                        g_Globals.timeNowMs, MemCurrentUsage()));

                if (Ent_RenderAll() != EOK) {
                        Panic("Failed to render entities");
                }
                R_EndCommands();

		R_RenderFrame();
		frameCount++;
	}

	if (Ent_Shutdown() != EOK) {
		Trace(CHAN_INFO, "failed to shutdown entity manager");
		return EFAIL;
	}

	if (R_Shutdown() != EOK) {
		Trace(CHAN_INFO, "failed to shutdown renderer");
		return EFAIL;
	}

	return EOK;
}
