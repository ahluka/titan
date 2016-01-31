#include "base.h"
#include "panic.h"
#include "memory.h"

typedef struct MemTag {
	struct MemTag *next;

	void *block;
	size_t blockSize;

	const char *file;
	long line;
	const char *func;
} MemTag;

#ifdef DEBUG_MEMORY_BUILD
static MemTag *s_Tags = NULL;
static uint64_t s_CurrentUsage = 0;
static uint64_t s_HighWater = 0;
#endif
static uint32_t s_AllocCount = 0;
static uint32_t s_FreeCount = 0;

#ifdef DEBUG_MEMORY_BUILD
static MemTag *FindTag(void *ptr)
{
	if (!s_Tags) return NULL;
	for (MemTag *i = s_Tags; i; i = i->next) {
		if (i->block == ptr) {
			return i;
		}
	}

	return NULL;
}

static void RemoveTag(MemTag *tag)
{
	for (MemTag *i = s_Tags, *prev = i; i; prev = i, i = i->next) {
		if (tag->block == i->block) {
			if (i == s_Tags) {
				s_Tags = i->next;
				free(i);
				return;
			}

			prev->next = i->next;
			free(i);
			return;
		}
	}
}
#endif /* DEBUG_MEMORY_BUILD */

/*
 * MemAlloc
 */
#ifdef DEBUG_MEMORY_BUILD
void *_MemAlloc(size_t sz, const char *file, long line, const char *fn)
#else
void *_MemAlloc(size_t sz)
#endif
{
	assert(sz > 0);

	uint8_t *bytes = calloc(1, sz);
#ifdef DEBUG_MEMORY_BUILD
	MemTag *tag = calloc(1, sizeof(*tag));
	tag->block = bytes;
	tag->blockSize = sz;
	tag->file = file;
	tag->line = line;
	tag->func = fn;
	tag->next = s_Tags;
	s_Tags = tag;

	s_CurrentUsage += sz;
	if (s_CurrentUsage > s_HighWater) {
		s_HighWater = s_CurrentUsage;
	}
#endif

	s_AllocCount++;
	return bytes;
}

/*
 * MemFree
 */
void _MemFree(void *ptr)
{
	if (!ptr) {
		/* Do nothing, just like free() */
		return;
	}
#ifdef DEBUG_MEMORY_BUILD
	MemTag *tag = FindTag(ptr);
	if (!tag) {
		Panic(Fmt("failed to find tag for %p", ptr));
	}

	s_CurrentUsage -= tag->blockSize;
	RemoveTag(tag);
#endif
	free(ptr);
	s_FreeCount++;
}
#ifdef DEBUG_MEMORY_BUILD
uint64_t MemCurrentUsage()
{
	return s_CurrentUsage;
}

uint64_t MemHighWater()
{
	return s_HighWater;
}

/*
 * MemStats
 *	Output a list of all memory blocks currently being tracked.
 *
 */
static bool SameAlloc(MemTag *tag, MemTag *prev)
{
	if (strcmp(tag->file, prev->file) == 0 &&
		strcmp(tag->func, prev->func) == 0 &&
		tag->line == prev->line &&
		tag->blockSize == prev->blockSize) {
		return true;
	}

	return false;
}

void MemStats()
{
	int same = 0;
	uint64_t htotal = 0;

	Trace(CHAN_MEM, "Memory blocks being tracked:");
	for (MemTag *i = s_Tags, *prev = i; i; prev = i, i = i->next) {
		if (i != s_Tags && SameAlloc(i, prev)) {
			same++;
			htotal += i->blockSize;
			continue;
		}

		if (same > 0) {
			Trace(CHAN_MEM, Fmt("  (%d hidden totalling %lu bytes)",
				same, htotal));
			same = 0;
			htotal = 0;
		}

		Trace(CHAN_MEM, Fmt(" %u bytes from %s:%ld in %s",
			i->blockSize, i->file, i->line, i->func));
	}
	Trace(CHAN_MEM, Fmt("Total: %lu bytes, highest: %lu bytes",
		s_CurrentUsage, s_HighWater));
}
#endif
uint32_t MemAllocCount()
{
	return s_AllocCount;
}

uint32_t MemFreeCount()
{
	return s_FreeCount;
}
