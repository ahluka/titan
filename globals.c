#include "base.h"
#include "globals.h"

Globals g_Globals;

void InitGlobals()
{
	if (g_Globals.initialised)
		return;

	g_Globals.initialised = true;
	g_Globals.debugTracingOn = true;
	g_Globals.timeNowMs = 0;
}
