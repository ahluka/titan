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
 * InitBase
 */
ecode_t InitBase()
{
        if (sstr_init() != EOK)
                return EFAIL;

	return EOK;
}

/*
 * ShutdownBase
 */
ecode_t ShutdownBase()
{
        if (sstr_shutdown() != EOK)
                return EFAIL;

	return EOK;
}
