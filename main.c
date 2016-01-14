#include "base.h"
#include "globals.h"
#include "panic.h"
#include "mtrand.h"
#include <time.h>
#include "gameloop.h"
#include "config.h"



#define CONFIG_FILENAME "config.ini"

/*
 * main
 */
int main(int argc, char **argv)
{
	InitGlobals();
	MTRSeed((uint64_t) time(NULL));

	if (LoadConfig(CONFIG_FILENAME) != EOK)
		Panic("Failed to load configuration");

	printf("%s version %s\n", g_Config.gameName, g_Config.version);

	if (Mainloop() != EOK)
		Panic("Failed to enter main loop");

	if (WriteConfig(CONFIG_FILENAME) != EOK)
		Panic("Failed to write configuration");

	return 0;
}
