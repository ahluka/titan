#include "base.h"
#include "globals.h"
#include "panic.h"
#include "mtrand.h"
#include <time.h>
#include "gameloop.h"
#include "config.h"
#include "files.h"

#define CONFIG_FILENAME "config.ini"

/*
 * main
 */
int main(int argc, char *argv[])
{
	InitGlobals();
	MTRSeed((uint64_t) time(NULL));

	if (Config_Load(CONFIG_FILENAME) != EOK)
		Panic("Failed to load configuration");

	if (Files_Init(g_Config.filesRoot) != EOK)
		Panic("Failed to init file system");

	printf("%s version %s\n", g_Config.gameName, g_Config.version);
	FileHandle hnd = Files_OpenFile("sheet.png");
	Files_ListOpen();
	Files_CloseFile(hnd);
	Files_ListOpen();

	if (Mainloop() != EOK)
		Panic("Failed to enter main loop");

	if (Files_Shutdown() != EOK)
		Panic("Failed to shutdown file system");

	if (Config_Save(CONFIG_FILENAME) != EOK)
		Panic("Failed to write configuration");

	return 0;
}
