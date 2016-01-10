#ifndef __GLOBALS_H__
#define __GLOBALS_H__

typedef struct Globals_s {
	bool initialised;

	const char *gameName;
	int verMajor;
	int verMinor;

	const char *windowTitle;
	int windowWidth;
	int windowHeight;
} Globals;

extern Globals g_Globs;

void InitGlobals();


#endif /* __GLOBALS_H__ */
