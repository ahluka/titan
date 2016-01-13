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

	printf("%s version %d.%d\n", g_Globals.gameName,
		g_Globals.verMajor, g_Globals.verMinor);

	if (LoadConfig(CONFIG_FILENAME) != EOK)
		Panic("Failed to load configuration");

	if (Mainloop() != EOK)
		Panic("Failed to enter main loop");

	if (WriteConfig(CONFIG_FILENAME) != EOK)
		Panic("Failed to write configuration");

	return 0;
}
