#include "base.h"
#include "memory.h"
#include "panic.h"
#include "list.h"

struct PoolNode {
	struct list_head list;
	void *block;
};

struct mem_pool {
	struct list_head freeBlocks;
	struct list_head usedBlocks;
	size_t blockSize;
	size_t blockCount;
	pool_policy_t policy;
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
 *	and increments the pool's blockCount accordingly.
 */
static void AddFreeNodes(mem_pool_t *pool, size_t count, size_t sz)
{
	if (sz != pool->blockSize) {
		panic(fmt("Block size mismatch in '%s'"));
	}

	for (size_t i = 0; i < count; i++) {
		struct PoolNode *node = NewPoolNode(sz);
                list_add(&node->list, &pool->freeBlocks);
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
static void *NextFreeBlock(mem_pool_t *pool)
{
	if (list_empty(&pool->freeBlocks)) {
		if (pool->policy == POOL_FIXEDSIZE) {
			panic(fmt("'%s' is empty", pool->debugName));
		} else {
			trace(CHAN_DBG, fmt("Resizing '%s'", pool->debugName));
			AddFreeNodes(pool, pool->blockCount, pool->blockSize);
		}
	}

        // FIXME: This is a stupid way to get the first element
        // Just grabbing the head with list_entry() didn't work, though?
        struct list_head *i, *safe;
        struct PoolNode *ret = NULL;
        list_for_each_safe(i, safe, &pool->freeBlocks) {
                ret = list_entry(i, struct PoolNode, list);
                list_move(i, &pool->usedBlocks);
                break;
        }

        return ret->block;
}

// FIXME: This can be removed eventually
UNUSED static void DebugPool(mem_pool_t *pool)
{
	size_t u = 0, f = 0;

        struct list_head *i = NULL;
        list_for_each(i, &pool->freeBlocks) {
                f++;
        }

        list_for_each(i, &pool->usedBlocks) {
                u++;
        }

	trace(CHAN_DBG, fmt("[%s] USED: %u, FREE: %u", pool->debugName, u, f));
}

/*
 * create_pool
 */
mem_pool_t *create_pool(size_t blockCount,
	size_t blockSize,
	pool_policy_t policy,
	const char *debugName)
{
	assert(blockSize > 0);
	assert(blockCount > 0);

	mem_pool_t *pool = MemAlloc(sizeof(*pool));
	pool->blockSize = blockSize;
	pool->policy = policy;
	pool->debugName = debugName;

        INIT_LIST_HEAD(&pool->freeBlocks);
        INIT_LIST_HEAD(&pool->usedBlocks);

	AddFreeNodes(pool, blockCount, blockSize);

	trace(CHAN_DBG, fmt("Pool '%s' %u x %u bytes", debugName,
		blockCount, blockSize));

	return pool;
}

/*
 * destroy_pool
 */
void destroy_pool(mem_pool_t *pool)
{
	assert(pool != NULL);

        trace(CHAN_DBG, fmt("pool '%s'", pool->debugName));

        struct PoolNode *i, *safe;
        list_for_each_entry_safe(i, safe, &pool->usedBlocks, list) {
                MemFree(i->block);
                MemFree(i);
        }

        list_for_each_entry_safe(i, safe, &pool->freeBlocks, list) {
                MemFree(i->block);
                MemFree(i);
        }

	MemFree(pool);
}

/*
 * PAlloc
 */
void *PAlloc(mem_pool_t *pool)
{
	assert(pool != NULL);

	void *ret = NextFreeBlock(pool);
	// DebugPool(pool);
	return ret;
}

/*
 * PFree
 */
void PFree(mem_pool_t *pool, void *block)
{
	assert(pool != NULL);
	assert(block != NULL);

        struct list_head *i, *safe;
        list_for_each_safe(i, safe, &pool->usedBlocks) {
                struct PoolNode *n = list_entry(i, struct PoolNode, list);

                if (n->block == block) {
                        memset(n->block, 0, pool->blockSize);
                        list_move(i, &pool->freeBlocks);
                        // DebugPool(pool);
                        return;
                }
        }

	panic("Invalid block");
}
