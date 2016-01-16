#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/* Any variables that will be needed by multiple modules should go here.
 * This includes flags passed to the executable.
 */
typedef struct Globals_s {
	bool initialised;

	bool debugTracingOn;

	/* Current game time in milliseconds */
	uint32_t timeNowMs;

	// TODO: Flags
} Globals;

extern Globals g_Globals;

void InitGlobals();


#endif /* __GLOBALS_H__ */
