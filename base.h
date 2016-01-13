#ifndef __BASE_H__
#define __BASE_H__

/*
 * The base include file, to be used in most modules.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


/*
 * Widely-used definitions.
 */
 /* Error codes
  *
  * Functions that might fail and are NOT predicates should return an ecode_t
  * to signify success or failure, never bool.
  */
typedef int ecode_t;
#define EOK	0
#define EFAIL	-1
/* TODO: Might define more of these? */

/* vsprintf() into a static buffer and return a pointer to it.
 * Very useful; avoids having to have varargs versions of functions that
 * accept arbitrary strings.
 */
const char *Fmt(const char *format, ...);


#endif /* __BASE_H__ */
