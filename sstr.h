/*
 * sstr.h
 *      Small string library. For handling common operations on small strings
 *      (anything like file names, etc) in an efficient way. Instead of
 *      allocating memory every time a string operation happens, we maintain
 *      a memory pool and use blocks from it to allocate space for strings
 *      quickly, so these functions can be used anywhere without worrying
 *      about potential performance problems.
 */

#pragma once

/* These are called by the base layer. */
ecode_t sstr_init();
ecode_t sstr_shutdown();

/* It's good form to free any char* results you get from functions in this
 * module, but they will be freed on shutdown regardless. */
void sstrfree(char *str);

/* Duplicate the given string. */
char *sstrdup(const char *str);

/* Concatenate the given strings. */
char *sstrcat(const char *first, const char *second);

/* Build a file name by (optionally) prepending a directory name to a file
 * name, and (optionally) appending a file extension.
 * dir and ext can be NULL.
 */
char *sstrfname(const char *dir, const char *name, const char *ext);
