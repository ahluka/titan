#include "base.h"
#include "memory.h"
#include <stdarg.h>

/*
 * Fmt
 */
const char *Fmt(const char *format, ...)
{
	va_list args;
	static char buffer[2][16000];
	static int index = 0;
	char *buf;

	buf = buffer[index & 1];
	index++;

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);

	return buf;
}

/*
 * StrDup
 */
char *StrDup(const char *str)
{
	char *ret = MemAlloc(strlen(str) + 1);
	strncpy(ret, str, strlen(str));
	return ret;
}
