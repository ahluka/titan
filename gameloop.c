#include "base.h"
#include "renderer.h"
#include <SDL2/SDL.h>

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

		RendererFrame();
	}
}
