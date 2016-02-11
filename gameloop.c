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
#include "cmds.h"
#include "input.h"
#include "event.h"
#include <SDL2/SDL.h>

/*
 * CheckDbgKeys
 *	Checks if a debug key was pressed and runs commands as necessary.
 */
static void CheckDbgKeys(SDL_Keycode code)
{
	switch (code) {
	case SDLK_F1:
		Cmd_ExecuteBuf("files-listopen");
		break;
	case SDLK_F2:
		Cmd_ExecuteBuf("cmdlist");
		break;
	case SDLK_F10:
		Trace(CHAN_MEM, Fmt("Current memory usage: %lu bytes",
			MemCurrentUsage()));
		break;
	case SDLK_F11:
		Trace(CHAN_MEM, Fmt("Highest memory usage: %lu bytes",
			MemHighWater()));
		break;
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
	struct Timer fpsTimer;
	struct Timer stepTimer;
	uint32_t frameCount = 0;

	Timer_Start(&fpsTimer);

	if (Rend_Init() != EOK) {
		Trace(CHAN_INFO, "failed to init renderer");
		return EFAIL;
	}

	if (Ent_Init() != EOK) {
		Trace(CHAN_INFO, "failed to init entity manager");
		return EFAIL;
	}

        Entity *test = Ent_Spawn("default");

        // struct property *prop = NULL;
        // list_for_each_entry(prop, &test->properties.props, list) {
        //         Trace(CHAN_DBG, Fmt("%s=%s", prop->key, prop->val));
        // }
        Ent_Free(test);

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

		// float avgFPS = frameCount / (Timer_GetTicks(&fpsTimer) / 1000.f);
		// static uint32_t nextOut = 0;
		//
		// if (nextOut <= g_Globals.timeNowMs) {
		// 	printf("%f\n", avgFPS);
		// 	nextOut = Timer_GetTicks(&fpsTimer) + 500;
		// }

		/* Update gameworld */
		float dT = Timer_GetTicks(&stepTimer) / 1000.f;
		g_Globals.timeNowMs = Timer_GetTicks(&fpsTimer);
		UpdateGameworld(dT);
		Timer_Start(&stepTimer); /* restart */

		/* Render */
		Rend_Frame();
		frameCount++;
	}

	if (Ent_Shutdown() != EOK) {
		Trace(CHAN_INFO, "failed to shutdown entity manager");
		return EFAIL;
	}

	if (Rend_Shutdown() != EOK) {
		Trace(CHAN_INFO, "failed to shutdown renderer");
		return EFAIL;
	}

	return EOK;
}
