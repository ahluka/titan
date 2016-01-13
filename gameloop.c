/*
 * gameloop module
 *	The main loop and gamestate updating.
 */
#include "base.h"
#include "renderer.h"
#include "timer.h"
#include "memory.h"
#include "globals.h"
#include "panic.h"
#include <SDL2/SDL.h>

/*
 * UpdateGameworld
 *	Update the gameworld state.
 */
void UpdateGameworld(float dT)
{
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
		Trace("Mainloop failed to init renderer");
		return EFAIL;
	}

	while (!quit) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT)
				quit = true;
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
			}
		}

		//float avgFPS = frameCount / (Timer_GetTicks(&fpsTimer) / 1000.f);

		/* Update gameworld */
		float dT = Timer_GetTicks(&stepTimer) / 1000.f;
		g_Globals.timeNowMs = Timer_GetTicks(&fpsTimer);
		UpdateGameworld(dT);
		Timer_Start(&stepTimer);

		/* Render */
		Rend_Frame();
		frameCount++;
	}

	if (Rend_Shutdown() != EOK) {
		Trace("Mainloop failed to shutdown renderer");
		return EFAIL;
	}

	return EOK;
}
