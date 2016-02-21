/*
 * TODO: Input module...
 *      Either fire off input events or just expose the key table for other
 *      modules to examine as and when?
 *              In_Update() to check key table once per frame and fire events?
 */
#include "base.h"
#include "panic.h"
#include "input.h"

// SDL_GetScancodeFromKey(event.key.keysym.sym)
// SDL_GetKeyFromScancode(event.key.keysym.scancode)

static bool s_Keys[SDL_NUM_SCANCODES] = {false};


void In_SetKeyDown(SDL_Scancode scode)
{
// 	trace(CHAN_DBG,
//		fmt("scancode %d keycode %d", scode,
//		SDL_GetKeyFromScancode(scode)));

	s_Keys[scode] = true;
}

void In_SetKeyUp(SDL_Scancode scode)
{
// 	trace(CHAN_DBG,
		// fmt("scancode %d keycode %d", scode,
		// SDL_GetKeyFromScancode(scode)));

	s_Keys[scode] = false;
}
