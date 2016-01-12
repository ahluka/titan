#include "base.h"
#include "config.h"
#include "panic.h"

struct ConfigData g_Config = {0};

/*
 * LoadConfig
 *	Load config data from the given file into g_Config.
 *	TODO: Detail structure of config files.
 */
ecode_t LoadConfig(const char *filename)
{
	Trace(Fmt("Using config file '%s'", filename));

	g_Config.filename = filename;

	return 0;
}

/*
 * WriteConfig
 *	Write g_Config into the given file.
 */
ecode_t WriteConfig(const char *filename)
{

	return 0;
}
