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

struct LAllocState {
        const char *name;
	uint8_t *base;
	uint8_t *current;
	size_t blockSize;
};

static MemTag *s_Tags = NULL;
static uint64_t s_CurrentUsage = 0;
static uint64_t s_HighWater = 0;

static uint32_t s_AllocCount = 0;
static uint32_t s_FreeCount = 0;

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

/*
 * MemAlloc
 */
void *_MemAlloc(size_t sz, const char *file, long line, const char *fn)
{
	assert(sz > 0);

	uint8_t *bytes = calloc(1, sz);
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

	MemTag *tag = FindTag(ptr);
	if (!tag) {
		panic(fmt("failed to find tag for %p", ptr));
	}

	s_CurrentUsage -= tag->blockSize;
	RemoveTag(tag);

	free(ptr);
	s_FreeCount++;
}

uint64_t MemCurrentUsage()
{
	return s_CurrentUsage;
}

uint64_t MemHighWater()
{
	return s_HighWater;
}

#define KB_BYTES 1024
#define MB_BYTES 1048576
uint32_t SaneVal(uint64_t v)
{
	if (v < KB_BYTES) return v;
	if (v >= KB_BYTES && v < MB_BYTES) return v / KB_BYTES;
	return v / MB_BYTES;
}

const char *SaneAff(uint32_t v)
{
	if (v < KB_BYTES) return "bytes";
	if (v >= KB_BYTES && v < MB_BYTES) return "KB";
	return "MB";
}
#undef MB_BYTES
#undef KB_BYTES

/*
 * MemStats
 *	Output a list of all memory blocks currently being tracked.
 *      Repeated lines of output are hidden, some get filtered entirely.
 */
static bool SameAlloc(MemTag *tag, MemTag *prev)
{
	if (strcmp(tag->file, prev->file) == 0 &&
		strcmp(tag->func, prev->func) == 0 &&
		/*tag->line == prev->line &&*/
		tag->blockSize == prev->blockSize) {
		return true;
	}

	return false;
}

/* Just a hack to make the debug output a bit more pleasant. */
static const char *filt[] = {"mem_pool.c"};
static int nfilt = sizeof(filt) / sizeof(filt[0]);

static bool is_filtered(const char *module)
{
        for (int i = 0; i < nfilt; i++) {
                if (strcmp(module, filt[i]) == 0)
                        return true;
        }

        return false;
}

void MemStats()
{
	int same = 0;
	uint64_t htotal = 0;
        uint64_t filtered = 0;

	trace(CHAN_MEM, "Memory blocks being tracked:");
	for (MemTag *i = s_Tags, *prev = i; i; prev = i, i = i->next) {
                if (is_filtered(i->file)) {
                        filtered++;
                        continue;
                }

		if (i != s_Tags && SameAlloc(i, prev)) {
			same++;
			htotal += i->blockSize;
			continue;
		}

		if (same > 0) {
			trace(CHAN_MEM, fmt("  (%d hidden totalling %u %s)",
				same, SaneVal(htotal), SaneAff(htotal)));
			same = 0;
			htotal = 0;
		}

		trace(CHAN_MEM, fmt(" %u %s from %s:%ld in %s",
			SaneVal(i->blockSize), SaneAff(i->blockSize),
			i->file, i->line, i->func));
	}
	trace(CHAN_MEM, fmt("Total: %u %s, highest: %u %s",
		SaneVal(s_CurrentUsage), SaneAff(s_CurrentUsage),
		SaneVal(s_HighWater), SaneAff(s_HighWater)));

        if (filtered > 0) {
                trace(CHAN_MEM, fmt("(%lu filtered) from:", filtered));
                for (int i = 0; i < nfilt; i++) {
                        trace(CHAN_MEM, fmt("\t%s", filt[i]));
                }
        }
}

uint32_t MemAllocCount()
{
	return s_AllocCount;
}

uint32_t MemFreeCount()
{
	return s_FreeCount;
}

/*
 * LAlloc_Create
 */
LAllocState *LAlloc_Create(size_t sz, const char *dbgName)
{
	assert(sz > 0);

	LAllocState *state = MemAlloc(sizeof(*state));
        if (!dbgName)
                state->name = "unnamed";
        else
                state->name = dbgName;

	state->base = MemAlloc(sz);
	state->current = state->base;
	state->blockSize = sz;
	return state;
}

/*
 * LAlloc_Destroy
 */
void LAlloc_Destroy(LAllocState *state)
{
	assert(state != NULL);
	MemFree(state->base);
	MemFree(state);
}

/*
 * LAlloc_Reset
 */
void LAlloc_Reset(LAllocState *state)
{
        assert(state != NULL);
        state->current = state->base;
}

/*
 * LAlloc
 */
void *LAlloc(LAllocState *state, size_t sz)
{
	assert(state != NULL);
	assert(sz > 0);

	if (state->current + sz > state->base + state->blockSize) {
		size_t left = state->blockSize - (state->current - state->base);
		panic(fmt("Cannot satisfy allocation of %u %s from %s (%u %s left)",
			SaneVal(sz), SaneAff(sz), state->name, SaneVal(left), SaneAff(left)));
	}

	void *ptr = state->current;
	state->current += sz;
	return ptr;
}
