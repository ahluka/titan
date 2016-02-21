#include "base.h"
#include "globals.h"
#include "panic.h"
#include "mtrand.h"
#include "gameloop.h"
#include "config.h"
#include "files.h"
#include "script.h"
#include "memory.h"
#include "event.h"
#include <time.h>
#include "vec.h"

#define CONFIG_FILENAME "config.ini"

/*
 * init_modules
 *	Call the initialisation functions of all modules in succession.
 *	panic() if one of them fails.
 */
static void init_modules()
{
	if (load_config(CONFIG_FILENAME) != EOK)
		panic("Failed to load configuration");

	if (init_files(g_Config.filesRoot) != EOK)
		panic("Failed to init file system");

	if (init_script() != EOK)
		panic("Failed to init script system");

	if (init_events() != EOK)
		panic("Failed to init event system");
}

/*
 * shutdown_modules
 *	Call the shutdown functions of all modules in succession. They should
 *	be called in reverse of the order in which their counterpart init
 *	functions were called in init_modules().
 */
static void shutdown_modules()
{
	if (shutdown_events() != EOK)
		panic("Failed to shutdown event system");

	if (shutdown_script() != EOK)
		panic("Failed to shutdown script system");

	if (shutdown_files() != EOK)
		panic("Failed to shutdown file system");

	if (save_config(CONFIG_FILENAME) != EOK)
		panic("Failed to write configuration");
}

/*
 * check_memory
 *	Gather memory usage stats and output a warning if everything isn't
 *	apparently hunky-dory. We want to see zero memory usage, and an
 *	equal number of MemAlloc()s and MemFree()s.
 */
static void check_memory()
{
	uint64_t usage = MemCurrentUsage();
	uint32_t allocs = MemAllocCount();
	uint32_t frees = MemFreeCount();
	uint32_t diff = allocs - frees;

	if (usage > 0 || diff != 0) {
		trace(CHAN_MEM, "====== WARNING ======");
		trace(CHAN_MEM, fmt("Memory usage on exit: %lu bytes", usage));
		trace(CHAN_MEM, fmt("%u allocs, %u frees (%u unaccounted for)",
			allocs, frees, diff));
		MemStats();
	}
}

static void run_tests()
{
	// event_t *evt = create_event("test-event", EVENT_BROADCAST | EVENT_QUEUED);
	// queue_event(evt);

}

/*
 * main
 *	Initialise everything then jump into the main loop. Clean up
 *	afterwards.
 */
int main(int argc, char *argv[])
{
	set_trace_channels(CHAN_ALL);

	init_base();
	init_globals();
	init_modules();
	init_random((uint32_t) time(NULL));

	trace(CHAN_INFO, fmt("%s version %s", g_Config.gameName, g_Config.version));
	run_tests();

	if (mainloop() != EOK)
		panic("Failed to enter main loop");

	shutdown_modules();
	shutdown_base();
	check_memory();

	return EXIT_SUCCESS;
}
