/*
 * mem_pool.h
 */
#pragma once

typedef enum pool_policy {
	POOL_FIXEDSIZE,
	POOL_DYNGROW
} pool_policy_t;

typedef struct mem_pool mem_pool_t;

mem_pool_t *create_pool(size_t blockCount,
	size_t blockSize,
	pool_policy_t policy,
	const char *debugName);
void destroy_pool(mem_pool_t *pool);
void *PAlloc(mem_pool_t *pool);
void PFree(mem_pool_t *pool, void *block);
