#include "base.h"
#include "config.h"
#include "panic.h"
#include "ini.h"
#include "memory.h"

struct ConfigData g_Config = {0};

/*
 * INIHandler
 *	ini_parse() calls this for every key=value pair it successfully
 *	parses.
 * usr: The pointer given to ini_parse(), NULL in this case.
 * sec: Section the key=value pair was found in.
 * Returns 1 back to ini_parse() to indicate success, 0 for failure.
 */
#define MATCH(s, k) strcmp(sec, s) == 0 && strcmp(key, k) == 0

static int
INIHandler(void *usr, const char *sec, const char *key, const char *val)
{
	Trace(CHAN_DBG, Fmt("handling [%s] %s=%s", sec, key, val));

	if (MATCH("general", "Game")) {
		g_Config.gameName = StrDup(val);
	} else if (MATCH("general", "Version")) {
		g_Config.version = StrDup(val);
	} else if (MATCH("renderer", "WindowWidth")) {
		g_Config.windowWidth = atoi(val);
	} else if (MATCH("renderer", "WindowHeight")) {
		g_Config.windowHeight = atoi(val);
	} else if (MATCH("filesystem", "FilesRoot")) {
		g_Config.filesRoot = StrDup(val);
	}

	return 1;
}
#undef MATCH

/*
 * Config_Load
 *	Load config data from the given file into g_Config.
 */
ecode_t Config_Load(const char *filename)
{
	assert(filename != NULL);
	Trace(CHAN_INFO, Fmt("Using config file '%s'", filename));

	g_Config.filename = filename;

	int ret = ini_parse(filename, INIHandler, NULL);
	if (ret < 0) {
		Trace(CHAN_INFO, Fmt("INI parse error %d\n", ret));
		return EFAIL;
	}

	return EOK;
}

/*
 * Config_Save
 *	Write g_Config into the given file and free it.
 */
ecode_t Config_Save(const char *filename)
{
	assert(filename != NULL);
	// TODO: Write it

	return EOK;
}
