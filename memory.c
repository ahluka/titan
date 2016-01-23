#include "base.h"
#include "panic.h"

/*
 * TODO: Some kind of memory usage tracking / book-keeping.
 */
#define TAG_MAGIC 0x717A717A

typedef struct MemTag {
	uint32_t magic;
	size_t blockSize;
} MemTag;

static uint64_t s_CurrentUsage = 0;
static uint64_t s_HighWater = 0;

/*
 * IsValidBlock
 *	Verifies that the given block of memory has been allocated through
 *	this interface.
 */
static bool IsValidBlock(void *block)
{
	uint8_t *p = (uint8_t *) block;
	MemTag tag = {0};

	memcpy(&tag, p, sizeof(tag));
	if (tag.magic != TAG_MAGIC) {
		return false;
	}

	return true;
}

/*
 * MemAlloc
 */
void *MemAlloc(size_t sz)
{
	uint8_t *bytes = NULL;

	if (sz == 0)
		Panic("Zero-length allocation");

	bytes = calloc(1, sz + sizeof(MemTag));
	if (bytes == NULL)
		Panic(Fmt("Failed to allocate %ld bytes", sz));

	MemTag tag = {TAG_MAGIC, sz};
	memcpy(bytes, &tag, sizeof(tag));

	//Trace(Fmt("allocated %u bytes", tag.blockSize));
	s_CurrentUsage += sz;
	if (s_CurrentUsage > s_HighWater) {
		s_HighWater = s_CurrentUsage;
	}

	return bytes + sizeof(tag);
}

/*
 * MemFree
 */
void MemFree(void *ptr)
{
	if (!ptr) {
		/* Do nothing, just like free() */
		return;
	}

	uint8_t *block = ((uint8_t *) ptr) - sizeof(MemTag);

	if (!IsValidBlock(block)) {
		Panic("Invalid block");
	}

	size_t sz = ((MemTag *) block)->blockSize;
	s_CurrentUsage -= sz;
	//Trace(Fmt("freeing %u bytes", sz));

	free(block);
}

uint64_t MemCurrentUsage()
{
	return s_CurrentUsage;
}

uint64_t MemHighWater()
{
	return s_HighWater;
}
