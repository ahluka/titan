#include "base.h"
#include "memory.h"
#include <stdarg.h>

/* StrDup() allocates its memory from this linear allocator. */
#define STRINGS_BUDGET 1024 * 512
static LAllocState *s_Strings = NULL;

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
	char *ret = LAlloc(s_Strings, strlen(str) + 1);
	strncpy(ret, str, strlen(str));
	return ret;
}

/*
 * InitBase
 */
ecode_t InitBase()
{
	if (!s_Strings) {
		s_Strings = LAlloc_Create(STRINGS_BUDGET);
	}

	return EOK;
}

/*
 * ShutdownBase
 */
ecode_t ShutdownBase()
{
	if (s_Strings != NULL) {
		LAlloc_Destroy(s_Strings);
		s_Strings = NULL;
	}

	return EOK;
}
