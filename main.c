#include "base.h"
#include "globals.h"
#include "panic.h"
#include "mtrand.h"
#include <time.h>
#include "gameloop.h"
#include "config.h"



#define CONFIG_FILENAME "config.txt"

/*
 * main
 */
int main(int argc, char **argv)
{
	InitGlobals();

	MTRSeed((uint64_t) time(NULL));

	printf("%s version %d.%d\n", g_Globs.gameName,
		g_Globs.verMajor, g_Globs.verMinor);

	if (LoadConfig(CONFIG_FILENAME) != 0)
		Panic("Failed to load configuration");

	Mainloop();

	if (WriteConfig(CONFIG_FILENAME) != 0)
		Panic("Failed to write configuration");

	return 0;
}
