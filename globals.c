#include "base.h"
#include "globals.h"

Globals g_Globs;

void InitGlobals()
{
	if (g_Globs.initialised)
		return;

	g_Globs.gameName = "Titan";
	g_Globs.verMajor = 0;
	g_Globs.verMinor = 1;

	g_Globs.windowTitle = "titan";
	g_Globs.windowWidth = 800;
	g_Globs.windowHeight = 600;

	g_Globs.initialised = true;
}

