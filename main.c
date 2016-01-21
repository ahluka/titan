#include "base.h"
#include "globals.h"
#include "panic.h"
#include "mtrand.h"
#include <time.h>
#include "gameloop.h"
#include "config.h"
#include "files.h"
#include "cmds.h"
#include "script.h"

#define CONFIG_FILENAME "config.ini"


/*
 * main
 *	TODO: refactor
 */
int main(int argc, char *argv[])
{
	InitGlobals();
	MTRSeed((uint64_t) time(NULL));

	/* NOTE: Cmd_Init() should be the first init function called; other
	 *	init functions might call Cmd_Register().
	 */
	if (Cmd_Init() != EOK)
		Panic("Failed to init command system");

	if (Config_Load(CONFIG_FILENAME) != EOK)
		Panic("Failed to load configuration");

	if (Files_Init(g_Config.filesRoot) != EOK)
		Panic("Failed to init file system");

	if (Script_Init() != EOK)
		Panic("Failed to init script system");

	printf("%s version %s\n", g_Config.gameName, g_Config.version);

	if (Mainloop() != EOK)
		Panic("Failed to enter main loop");

	if (Script_Shutdown() != EOK)
		Panic("Failed to shutdown script system");

	if (Files_Shutdown() != EOK)
		Panic("Failed to shutdown file system");

	if (Config_Save(CONFIG_FILENAME) != EOK)
		Panic("Failed to write configuration");

	if (Cmd_Shutdown() != EOK)
		Panic("Failed to shutdown command system");

	return EXIT_SUCCESS;
}
