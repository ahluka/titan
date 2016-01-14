#include "base.h"
#include "panic.h"

/*
 * TODO: Some kind of memory usage tracking / book-keeping.
 */

/*
 * MemAlloc
 */
void *MemAlloc(size_t sz)
{
	void *bytes = NULL;

	if (sz == 0)
		Panic("Zero-length allocation");

	bytes = calloc(1, sz);
	if (bytes == NULL)
		Panic(Fmt("Failed to allocate %ld bytes", sz));

	return bytes;
}

/*
 * MemFree
 */
void MemFree(void *ptr)
{
	free(ptr);
}
