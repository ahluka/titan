#include "base.h"
#include "config.h"
#include "panic.h"
#include "ini.h"
#include "memory.h"

struct ConfigData g_Config = {0};

/*
 * handler
 *	ini_parse() calls this for every key=value pair it successfully
 *	parses.
 * usr: The pointer given to ini_parse(), NULL in this case.
 * sec: Section the key=value pair was found in.
 * Returns 1 back to ini_parse() to indicate success, 0 for failure.
 */
#define MATCH(s, k) strcmp(sec, s) == 0 && strcmp(key, k) == 0

static int
handler(void *usr, const char *sec, const char *key, const char *val)
{
	// trace(CHAN_DBG, fmt("handling [%s] %s=%s", sec, key, val));

	if (MATCH("General", "Game")) {
		g_Config.gameName = sstrdup(val);
	} else if (MATCH("General", "Version")) {
		g_Config.version = sstrdup(val);
	} else if (MATCH("Renderer", "WindowWidth")) {
		g_Config.windowWidth = atoi(val);
	} else if (MATCH("Renderer", "WindowHeight")) {
		g_Config.windowHeight = atoi(val);
        } else if (MATCH("Renderer", "VSync")) {
                char *dup = sstrdup_lower(val);
                if (strcmp(dup, "true") == 0 || strcmp(dup, "on") == 0)
                        g_Config.vsync = true;
                else
                        g_Config.vsync = false;
                sstrfree(dup);
	} else if (MATCH("FileSystem", "FilesRoot")) {
		g_Config.filesRoot = sstrdup(val);
	}

	return 1;
}
#undef MATCH

/*
 * load_config
 *	Load config data from the given file into g_Config.
 */
ecode_t load_config(const char *filename)
{
	assert(filename != NULL);
	trace(CHAN_INFO, fmt("Using config file '%s'", filename));

	g_Config.filename = filename;

	int ret = ini_parse(filename, handler, NULL);
	if (ret < 0) {
		trace(CHAN_INFO, fmt("INI parse error %d\n", ret));
		return EFAIL;
	}

	return EOK;
}

/*
 * save_config
 *	Write g_Config into the given file and free any memory it used.
 */
ecode_t save_config(const char *filename)
{
	assert(filename != NULL);
	// TODO: Write it

        sstrfree(g_Config.gameName);
        sstrfree(g_Config.version);
        sstrfree(g_Config.filesRoot);

	return EOK;
}
