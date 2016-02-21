#include "base.h"
#include "memory.h"
#include <stdarg.h>


/*
 * fmt
 */
const char *fmt(const char *format, ...)
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
 * init_base
 */
ecode_t init_base()
{
        if (sstr_init() != EOK)
                return EFAIL;

	return EOK;
}

/*
 * shutdown_base
 */
ecode_t shutdown_base()
{
        if (sstr_shutdown() != EOK)
                return EFAIL;

	return EOK;
}
