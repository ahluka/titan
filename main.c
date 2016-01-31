#include "base.h"
#include "globals.h"
#include "panic.h"
#include "mtrand.h"
#include "gameloop.h"
#include "config.h"
#include "files.h"
#include "cmds.h"
#include "script.h"
#include "memory.h"
#include <time.h>

#define CONFIG_FILENAME "config.ini"

// TODO: Pool and linear allocators

/*
 * InitModules
 *	Call the initialisation functions of all modules in succession.
 *	Panic() if one of them fails.
 */
static void InitModules()
{
	/* NOTE: Cmd_Init() should be the first init function called; other
	 * modules might call into it during initialisation.
	 */
	if (Cmd_Init() != EOK)
		Panic("Failed to init command system");

	if (Config_Load(CONFIG_FILENAME) != EOK)
		Panic("Failed to load configuration");

	if (Files_Init(g_Config.filesRoot) != EOK)
		Panic("Failed to init file system");

	if (Script_Init() != EOK)
		Panic("Failed to init script system");
}

/*
 * ShutdownModules
 *	Call the shutdown functions of all modules in succession. They should
 *	be called in reverse of the order in which their counterpart init
 *	functions were called in InitModules().
 */
static void ShutdownModules()
{
	if (Script_Shutdown() != EOK)
		Panic("Failed to shutdown script system");

	if (Files_Shutdown() != EOK)
		Panic("Failed to shutdown file system");

	if (Config_Save(CONFIG_FILENAME) != EOK)
		Panic("Failed to write configuration");

	if (Cmd_Shutdown() != EOK)
		Panic("Failed to shutdown command system");
}

/*
 * CheckMemory
 *	Gather memory usage stats and output a warning if everything isn't
 *	apparently hunky-dory. We want to see zero memory usage, and an
 *	equal number of MemAlloc()s and MemFree()s.
 */
static void CheckMemory()
{
	uint64_t usage = MemCurrentUsage();
	uint32_t allocs = MemAllocCount();
	uint32_t frees = MemFreeCount();
	uint32_t diff = allocs - frees;

	if (usage > 0 || diff != 0) {
		Trace(CHAN_MEM, "====== WARNING ======");
		Trace(CHAN_MEM, Fmt("Memory usage on exit: %lu bytes", usage));
		Trace(CHAN_MEM, Fmt("%u allocs, %u frees (%u unaccounted for)",
			allocs, frees, diff));
		MemStats();
	}
}

/*
 * main
 *	Initialise everything then jump into the main loop. Clean up
 *	afterwards.
 */
int main(int argc, char *argv[])
{
	SetTraceChannels(CHAN_ALL);

	InitGlobals();
	InitModules();
	Random_Init((uint32_t) time(NULL));

	printf("%s version %s\n", g_Config.gameName, g_Config.version);

	if (Mainloop() != EOK)
		Panic("Failed to enter main loop");

	ShutdownModules();

	CheckMemory();
	return EXIT_SUCCESS;
}
