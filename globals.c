#include "base.h"
#include "globals.h"

Globals g_Globals;

void InitGlobals()
{
	if (g_Globals.initialised)
		return;

	g_Globals.gameName = "Titan";
	g_Globals.verMajor = 0;
	g_Globals.verMinor = 1;

	g_Globals.windowTitle = "titan";
	g_Globals.windowWidth = 800;
	g_Globals.windowHeight = 600;

	g_Globals.initialised = true;
}
