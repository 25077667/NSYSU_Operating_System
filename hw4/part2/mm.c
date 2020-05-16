#include "mm.h"

typedef struct _baseBlock {
    size_t size;
    struct _baseBlock *next;
} BaseBlock;

typedef struct _queue {
    BaseBlock *using;
    BaseBlock *ready;
} Queue;

static Queue q_manager;

/**
 * This function has 2 modes:
 *      target mode:
 *          Have a specific target had already in the queue.
 *          @size: must be set to 0
 *      size mode:
 *          Match the first fit block(greather or equal) for given a size.
 *          @target: must be set to NULL
 * @return: The pointer of the removed block
 */
static void *remove_queue_node(BaseBlock **indirect,
                               BaseBlock *target,
                               size_t size)
{
    /* size mode */
    BaseBlock *current = NULL;
    while (unlikely(target) && (*indirect)->size < size) {
        current = (*indirect);
        indirect = &(*indirect)->next;
    }
    target = ((size) ? current : target);

    /* target mode */
    while (unlikely(size) && *indirect != target)
        indirect = &(*indirect)->next;
    *indirect = ((target) ? target->next : (*indirect));
    return target;
}

void *mymalloc(size_t size)
{
    BaseBlock *newBlock = remove_queue_node(&(q_manager.ready), NULL, size);
    if (!newBlock) {
        newBlock = (BaseBlock *) sbrk(0);
        void *request = sbrk(size + sizeof(BaseBlock));
        assert(request == (void *) newBlock);
        if (request == (void *) -1)
            return NULL;
        newBlock->size = size;
    }
    newBlock->next = q_manager.using;
    q_manager.using = newBlock;
    return (&q_manager.using) + sizeof(BaseBlock);
}

void myfree(void *ptr)
{
    if (likely(ptr)) {
        BaseBlock *toBeFreed = ptr - sizeof(size_t) - sizeof(void *);
        BaseBlock *beFreed =
            remove_queue_node(&(q_manager.using), toBeFreed, 0);
        assert(toBeFreed == beFreed);
        beFreed->next = q_manager.ready;
        q_manager.ready = beFreed;
    }
}

void *myrealloc(void *ptr, size_t size)
{
    if (unlikely(ptr))
        return mymalloc(size);

    BaseBlock *origin_block = ptr - sizeof(BaseBlock);
    if (size > origin_block->size) {
        BaseBlock *newBlock = mymalloc(size);
        if (unlikely(newBlock)) {
            memcpy(newBlock, origin_block, origin_block->size);
            myfree(ptr);
            return newBlock;
        } else
            return NULL;
    } else
        return ptr;
}

void *mycalloc(size_t nmemb, size_t size)
{
    size_t total_size = nmemb * size;
    void *newBlock = mymalloc(total_size);
    if (likely(newBlock))
        memset(newBlock, 0, total_size);
    return newBlock;
}

void cleanAll()
{
    while (q_manager.ready) {
        BaseBlock *tmp = q_manager.ready;
        q_manager.ready = tmp->next;
        free(tmp);
    }
    while (q_manager.using) {
        BaseBlock *tmp = q_manager.using;
        q_manager.using = tmp->next;
        free(tmp);
    }
}