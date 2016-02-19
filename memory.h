/*
 * memory.h
 *	All memory allocations should go through MemAlloc(), frees through
 *	MemFree(). This module also provides a linear allocator API.
 */
#pragma once
#include "mem_pool.h"

#define MemAlloc(sz) _MemAlloc(sz, __FILE__, __LINE__, __func__)
#define MemFree(ptr) _MemFree(ptr)

void *_MemAlloc(size_t sz, const char *file, long line, const char *fn);
void _MemFree(void *ptr);


/* Debugging */
uint32_t SaneVal(uint64_t v);
const char *SaneAff(uint32_t v);
uint64_t MemCurrentUsage();
uint64_t MemHighWater();
void MemStats();
uint32_t MemAllocCount();
uint32_t MemFreeCount();

/*
 * Linear allocator API
 *	Use LAlloc_Create() to create a new linear allocator of the given size.
 *	The LAllocState pointer returned is used to calls to the rest of the
 *	API.
 *
 *	A linear allocator is a simple way to manage allocations into one
 *	contiguous block of memory. It's extremely fast, since you're only
 *	doing some simple pointer manipulations, but as a tradeoff you can
 *	only free blocks in reverse order to which you allocated them. Or
 *	you just do what this API does and disallows freeing completely.
 *	Just LAlloc_Destroy() when you're done.
 *
 *	TODO: Freeing to markers?
 */
typedef struct LAllocState LAllocState;

LAllocState *LAlloc_Create(size_t sz, const char *dbgName);
void LAlloc_Destroy(LAllocState *state);
void LAlloc_Reset(LAllocState *state);

/* Returns a pointer to a memory block of the given size from the memory
 * managed by the given allocator state, or Panic()s and doesn't return if
 * there isn't enough memory to satisfy.
 */
void *LAlloc(LAllocState *state, size_t sz);
