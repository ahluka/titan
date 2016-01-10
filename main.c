#include "base.h"
#include "globals.h"
#include "renderer.h"
#include "panic.h"

/* TODO
 * - Hash table? Or at least hash function.
 * - Config with g_Conf interface.
 * - 
 */

/*
 * LoadResources
 */
bool LoadResources()
{

	return true;
}



/*
 * main
 */
int main(int argc, char **argv)
{
	InitGlobals();

	printf("%s version %d.%d\n", g_Globs.gameName,
		g_Globs.verMajor, g_Globs.verMinor);

	if (!LoadResources()) {
		Panic("Failed to load resources!");
	}

	RendererInit();
	RendererFrame();
	RendererShutdown();

	return 0;
}
