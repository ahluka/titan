#include "base.h"
#include "memory.h"
#include "panic.h"

struct PoolNode {
	struct PoolNode *next;
	void *block;
};

struct MemPool {
	struct PoolNode *freeBlocks;
	struct PoolNode *usedBlocks;
	size_t blockSize;
	size_t blockCount;
	PoolPolicy policy;
	const char *debugName;
};

static struct PoolNode *NewPoolNode(size_t blocksz)
{
	struct PoolNode *node = MemAlloc(sizeof(*node));
	node->block = MemAlloc(blocksz);
	return node;
}

/*
 * AddFreeNodes
 *	Adds the specified number of new nodes to the pool's free list,
 *	and increments the pool's blockCount.
 */
static void AddFreeNodes(MemPool *pool, size_t count, size_t sz)
{
	if (sz != pool->blockSize) {
		Panic(Fmt("Block size mismatch in '%s'"));
	}

	for (size_t i = 0; i < count; i++) {
		struct PoolNode *node = NewPoolNode(sz);
		node->next = pool->freeBlocks;
		pool->freeBlocks = node;
	}

	pool->blockCount += count;
}

/*
 * NextFreeBlock
 *	Takes a free block from the pool's free list, moves it onto the pool's
 *	used list, and returns a pointer to it.
 *	If there are no free blocks available and the pool's policy is
 *	POOL_DYNGROW, then we double the pool's size with AddFreeNodes().
 */
static void *NextFreeBlock(MemPool *pool)
{
	if (pool->freeBlocks == NULL) {
		if (pool->policy == POOL_FIXEDSIZE) {
			Panic(Fmt("'%s' is empty", pool->debugName));
		} else {
			Trace(CHAN_DBG, Fmt("Resizing '%s'", pool->debugName));
			AddFreeNodes(pool, pool->blockCount, pool->blockSize);
		}
	}

	struct PoolNode *ret = pool->freeBlocks;
	pool->freeBlocks = pool->freeBlocks->next;

	ret->next = pool->usedBlocks;
	pool->usedBlocks = ret;

	return ret->block;
}

// FIXME: This can be removed eventually
static void DebugPool(MemPool *pool)
{
	size_t u = 0, f = 0;

	for (struct PoolNode *i = pool->usedBlocks; i; i = i->next, u++)
		;

	for (struct PoolNode *i = pool->freeBlocks; i; i = i->next, f++)
		;

	Trace(CHAN_DBG, Fmt("%s - USED: %u, FREE: %u", pool->debugName, u, f));
}

/*
 * Pool_Create
 */
MemPool *Pool_Create(size_t blockCount,
	size_t blockSize,
	PoolPolicy policy,
	const char *debugName)
{
	assert(blockSize > 0);
	assert(blockCount > 0);

	MemPool *pool = MemAlloc(sizeof(*pool));
	pool->blockSize = blockSize;
	pool->policy = policy;
	pool->debugName = debugName;

	AddFreeNodes(pool, blockCount, blockSize);

	Trace(CHAN_DBG, Fmt("Pool '%s' %u x %u bytes", debugName,
		blockCount, blockSize));

	return pool;
}

/*
 * Pool_Destroy
 */
void Pool_Destroy(MemPool *pool)
{
	assert(pool != NULL);

	struct PoolNode *i = pool->usedBlocks;
	struct PoolNode *dead = NULL;

	while (i) {
		dead = i;
		i = i->next;
		MemFree(dead->block);
		MemFree(dead);
	}

	i = pool->freeBlocks;
	while (i) {
		dead = i;
		i = i->next;
		MemFree(dead->block);
		MemFree(dead);
	}

	MemFree(pool);
}

/*
 * PAlloc
 */
void *PAlloc(MemPool *pool)
{
	assert(pool != NULL);

	void *ret = NextFreeBlock(pool);
	DebugPool(pool);
	return ret;
}

/*
 * PFree
 */
void PFree(MemPool *pool, void *block)
{
	assert(pool != NULL);
	assert(block != NULL);

	/* Is this block ours to free? */
	for (struct PoolNode *i = pool->usedBlocks, *prev = i;
		i; prev = i, i = i->next) {
		if (i->block == block) {
			/* Yes; move it onto the pool's free list. */
			if (i == pool->usedBlocks) {
				pool->usedBlocks = NULL;
			} else {
				prev->next = i->next;
			}

			i->next = pool->freeBlocks;
			pool->freeBlocks = i;
			return;
		}
	}

	/* No; shit ourselves. */
	Panic("Invalid block");
}
