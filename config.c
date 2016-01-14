#include "base.h"
#include "config.h"
#include "panic.h"
#include "ini.h"

struct ConfigData g_Config = {0};

/*
 * INIHandler
 *	ini_parse() calls this for every key=value pair it successfully
 *	parses.
 * usr: The pointer given to ini_parse(), NULL in this case.
 * sec: Section the key=value pair was found in.
 * Returns 1 back to ini_parse() to indicate success, 0 for failure.
 */
static int
INIHandler(void *usr, const char *sec, const char *key, const char *val)
{
#define MATCH(s, k) strcmp(sec, s) == 0 && strcmp(key, k) == 0
	if (MATCH("general", "Game")) {
		g_Config.gameName = strdup(val);
	} else if (MATCH("general", "Version")) {
		g_Config.version = strdup(val);
	} else if (MATCH("renderer", "WindowWidth")) {
		g_Config.windowWidth = atoi(val);
	} else if (MATCH("renderer", "WindowHeight")) {
		g_Config.windowHeight = atoi(val);
	}

	return 1;
}

/*
 * LoadConfig
 *	Load config data from the given file into g_Config.
 */
ecode_t LoadConfig(const char *filename)
{
	Trace(Fmt("Using config file '%s'", filename));
	g_Config.filename = filename;

	int ret = ini_parse(filename, INIHandler, NULL);
	if (ret < 0) {
		Trace(Fmt("INI parse error %d\n", ret));
		return EFAIL;
	}

	return EOK;
}

/*
 * WriteConfig
 *	Write g_Config into the given file and free it.
 */
ecode_t WriteConfig(const char *filename)
{
	// TODO: Write it

	/* free strings from strdup() */
	free(g_Config.gameName);
	free(g_Config.version);

	return EOK;
}
