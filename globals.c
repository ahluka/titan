#include "base.h"
#include "globals.h"

struct globals g_globals;

void init_globals()
{
	if (g_globals.initialised)
		return;

	g_globals.initialised = true;
	g_globals.debugTracingOn = true;
	g_globals.timeNowMs = 0;
}
