/*
	TODO: All this crap...

	either fire off input events, or just expose an input table for other
	modules to examine as and when?
		In_Update to check s_Keys once per frame and fire events for
		keys that are down?
*/
#include "base.h"
#include "panic.h"
#include "input.h"

// SDL_GetScancodeFromKey(event.key.keysym.sym)
// SDL_GetKeyFromScancode(event.key.keysym.scancode)

static bool s_Keys[SDL_NUM_SCANCODES] = {false};


void In_SetKeyDown(SDL_Scancode scode)
{
// 	Trace(CHAN_DBG,
//		Fmt("scancode %d keycode %d", scode,
//		SDL_GetKeyFromScancode(scode)));

	s_Keys[scode] = true;
}

void In_SetKeyUp(SDL_Scancode scode)
{
// 	Trace(CHAN_DBG,
		// Fmt("scancode %d keycode %d", scode,
		// SDL_GetKeyFromScancode(scode)));

	s_Keys[scode] = false;
}
