/*
	TODO
	update an input table for keys?
	switchable hardcoded debugging keys

	either fire off input events, or just expose an input table for other
	modules to examine as and when?
		In_Update to check s_Keys once per frame and fire events for
		keys that are down?
*/
#include "base.h"
#include "panic.h"
#include "cmds.h"
#include "input.h"

// SDL_GetScancodeFromKey(event.key.keysym.sym)
// SDL_GetKeyFromScancode(event.key.keysym.scancode)

static bool s_Keys[SDL_NUM_SCANCODES] = {false};


void In_KeyDown(SDL_Scancode scode)
{
#ifdef DEBUG_TRACING_ON
	Trace(Fmt("scancode %d keycode %d", scode, SDL_GetKeyFromScancode(scode)));
#endif

	s_Keys[scode] = true;
}

void In_KeyUp(SDL_Scancode scode)
{
#ifdef DEBUG_TRACING_ON
	Trace(Fmt("scancode %d keycode %d", scode, SDL_GetKeyFromScancode(scode)));
#endif
	s_Keys[scode] = false;
}
