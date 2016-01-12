#include "base.h"
#include <stdarg.h>

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
