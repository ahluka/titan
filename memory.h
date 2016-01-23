#pragma once

/*
 * All memory allocations should go through MemAlloc().
 * MemFree() exists for symmetry but doesn't do anything other than call free().
 */

void *MemAlloc(size_t sz);
void MemFree(void *ptr);
uint64_t MemCurrentUsage();
uint64_t MemHighWater();
