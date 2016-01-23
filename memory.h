#pragma once

/*
 * memory.h
 *	All memory allocations should go through MemAlloc(), frees through
 *	MemFree().
 */

/* Comment this out to disable detailed memory usage tracking.
 * Without this defined, only allocation and free counts are tracked.
 */
#define DEBUG_MEMORY_BUILD

#ifdef DEBUG_MEMORY_BUILD
#define MemAlloc(sz) _MemAlloc(sz, __FILE__, __LINE__, __func__)
#define MemFree(ptr) _MemFree(ptr)

void *_MemAlloc(size_t sz, const char *file, long line, const char *fn);
void _MemFree(void *ptr);
#else
#define MemAlloc(sz) _MemAlloc(sz);
#define MemFree(ptr) _MemFree(ptr);

void *_MemAlloc(size_t sz);
void _MemFree(void *ptr);
#endif

/* Debugging */
#ifdef DEBUG_MEMORY_BUILD
uint64_t MemCurrentUsage();
uint64_t MemHighWater();
void MemStats();
#endif
uint32_t MemAllocCount();
uint32_t MemFreeCount();
