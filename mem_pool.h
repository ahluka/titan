/*
 * mem_pool.h
 */
#pragma once

typedef enum PoolPolicy {
	POOL_FIXEDSIZE,
	POOL_DYNGROW
} PoolPolicy;

typedef struct MemPool MemPool;

MemPool *Pool_Create(size_t blockCount,
	size_t blockSize,
	PoolPolicy policy,
	const char *debugName);
void Pool_Destroy(MemPool *pool);
void *PAlloc(MemPool *pool);
void PFree(MemPool *pool, void *block);
