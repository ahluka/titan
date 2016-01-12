/*
 * gameloop module
 *	The main loop and gamestate updating.
 */
#include "base.h"
#include "renderer.h"
#include "timer.h"
#include "memory.h"
#include <SDL2/SDL.h>

/*
 * UpdateGameworld
 *	Update the gameworld state.
 */
void UpdateGameworld(uint32_t time, float dT)
{
}

/*
 * Enter the main loop. Each iteration:
 *	- Read user input
 *	- Update gameworld (run AI, etc.)
 *	- Render view
 */
void Mainloop()
{
	SDL_Event event;
	bool quit = false;
	struct Timer fpsTimer;
	struct Timer stepTimer;
	uint32_t frameCount = 0;

	Timer_Start(&fpsTimer);
	Rend_Init();

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

		float avgFPS = frameCount / (Timer_GetTicks(&fpsTimer) / 1000.f);
		float dT = Timer_GetTicks(&stepTimer) / 1000.f;

		printf("FPS: %f, dT: %f\n", avgFPS, dT);

		UpdateGameworld(Timer_GetTicks(&fpsTimer), dT);
		Timer_Start(&stepTimer);

		Rend_Frame();
		frameCount++;
	}

	Rend_Shutdown();
}
