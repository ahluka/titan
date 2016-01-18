/*
 * gameloop
 *	The main loop and gamestate updating.
 */
#include "base.h"
#include "renderer.h"
#include "timer.h"
#include "memory.h"
#include "globals.h"
#include "panic.h"
#include "entity.h"
#include "cmds.h"
#include <SDL2/SDL.h>

/*
 * UpdateGameworld
 *	Update the gameworld state. This basically involves letting all
 *	entities update themselves.
 */
static void UpdateGameworld(float dT)
{
	if (Ent_UpdateAll(dT) != EOK)
		Panic("Failed to update Entities");
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
		Trace("failed to init renderer");
		return EFAIL;
	}

	if (Ent_Init() != EOK) {
		Trace("failed to init entity manager");
		return EFAIL;
	}

	while (!quit) {
		/* input */
		while (SDL_PollEvent(&event) != 0) {
			// TODO: This needs to call into an input module, etc.
			if (event.type == SDL_QUIT)
				quit = true;
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				case SDLK_F12:
					Cmd_Execute("cmdlist");
					break;
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
		Timer_Start(&stepTimer);

		/* Render */
		Rend_Frame();
		frameCount++;
	}

	if (Ent_Shutdown() != EOK) {
		Trace("failed to shutdown entity manager");
		return EFAIL;
	}

	if (Rend_Shutdown() != EOK) {
		Trace("failed to shutdown renderer");
		return EFAIL;
	}

	return EOK;
}
