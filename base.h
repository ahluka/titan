#ifndef __BASE_H__
#define __BASE_H__

/*
 * The base include file, to be used in most modules.
 * Probably best to include this first.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// TODO: Uncomment this for release
// #define NDEBUG
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
const char *fmt(const char *format, ...);


ecode_t init_base();
ecode_t shutdown_base();

#include "sstr.h"

#endif /* __BASE_H__ */
