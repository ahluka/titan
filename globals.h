#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/* Any variables that will be needed by multiple modules should go here.
 * This includes flags passed to the executable.
 */
struct globals {
	bool initialised;

	bool debugTracingOn;

	/* Current game time in milliseconds */
	uint32_t timeNowMs;

	// TODO: program flags
};

extern struct globals g_globals;

#define TIMENOW_PLUS(ms) g_globals.timeNowMs + (ms)

void init_globals();


#endif /* __GLOBALS_H__ */
