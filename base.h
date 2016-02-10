#ifndef __BASE_H__
#define __BASE_H__

/*
 * The base include file, to be used in most modules.
 * Probably best to include this first in the modules that use it.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
// TODO: Uncomment this for release
//#define NDEBUG
#include <assert.h>

/* Place a breakpoint / trap instruction for debugging. */
#define BREAKPOINT do { __asm__("int $0x3"); } while (0)

#define UNUSED __attribute__((unused))

/* Error codes
 *
 * Functions that might fail and are NOT predicates should return an ecode_t
 * to signify success or failure, never bool.
 */
typedef int ecode_t;
#define EOK	0
#define EFAIL	-1

/* vsprintf() into a static buffer and return a pointer to it.
 * Very useful; avoids having to have varargs versions of functions that
 * accept arbitrary strings.
 */
const char *Fmt(const char *format, ...);

/* Own implementation of strdup() that allocates from the global string
 * pool.
 * NOTE: Unlike with strdup() from the standard library, you DO NOT free
 * the pointers returned by this function. They're freed all at once when
 * the base layer shuts down.
 */
char *StrDup(const char *str);

ecode_t InitBase();
ecode_t ShutdownBase();


#endif /* __BASE_H__ */
