#include "mm.h"

typedef struct _baseBlock {
    size_t size;
    struct _baseBlock *next;
} BaseBlock;

typedef struct _queue {
    BaseBlock *using;
    BaseBlock *freeList;
} Queue;

/*
 * A memory pool that using linked-list to link all allocated and freed spaces.
 *
 * @using: Contain all the user allocation object in this linked-list.
 *      + Why I need to record using space?
 *          Actually, it's just a record to make "debug" easier.
 *          This linked-list(Pool) only be used while `printMallocSpace()`
 *          calling. So, You can remove it if you don't need the
 *          `printMallocSpace()`.
 *
 * @freeList: Contain all the user freed space which had been allocated, also is
 * a linked-list.
 *      + Reference: https://en.wikipedia.org/wiki/Free_list
 */
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
    /* The indirect (linked-list) is not exist */
    if (!(*indirect))
        return NULL;

    /* size mode */
    while (!(target) && (*indirect) && (*indirect)->size < size)
        indirect = &(*indirect)->next;
    target = ((size) ? (*indirect) : target);

    /* target mode */
    while (!(size) && *indirect != target)
        indirect = &(*indirect)->next;

    *indirect = ((target) ? target->next : (*indirect));
    return target;
}

void *mymalloc(size_t size)
{
    /* Check the freeList space have a space allocated */
    BaseBlock *newBlock = remove_queue_node(&(q_manager.freeList), NULL, size);
    if (!newBlock) {
        newBlock = (BaseBlock *) sbrk(0);
        void *request = sbrk(size + sizeof(BaseBlock));
        /* Allocation failed! */
        if (request == (void *) -1)
            return NULL;
        newBlock->size = size;
    }
    newBlock->next = q_manager.using;
    q_manager.using = newBlock;
    return q_manager.using + 1;
}

void myfree(void *ptr)
{
    if (likely(ptr)) {
        BaseBlock *toBeFreed = ptr - sizeof(BaseBlock);
        BaseBlock *beFreed =
            remove_queue_node(&(q_manager.using), toBeFreed, 0);
        beFreed->next = q_manager.freeList;
        q_manager.freeList = beFreed;
    }
}

void *myrealloc(void *ptr, size_t size)
{
    if (!(ptr))
        return mymalloc(size);

    BaseBlock *origin_block = ptr - sizeof(BaseBlock);

    /* The origin space is enough to extand to request */
    if (size <= origin_block->size)
        return ptr;

    /* Try malloc new space, and mount old to freeList chain */
    void *newBlock = mymalloc(size + sizeof(BaseBlock));
    if (newBlock) {
        memset(newBlock, 0, size);
        memcpy(newBlock, origin_block, origin_block->size);
        myfree(ptr);
    }
    return newBlock;
}

/* malloc and clear */
void *mycalloc(size_t nmemb, size_t size)
{
    size_t total_size = nmemb * size;
    void *newBlock = mymalloc(total_size);
    if (newBlock)
        memset(newBlock, 0, total_size);
    return newBlock;
}

/**
 * Display allocation infomation about mm.h.
 *
 * Display format:
 *  address[size] -> address[size] -> address[size] -> ...  address[size] ->
 * @total_alloced: the numbers of blocks(not the size).
 */
void printMallocSpace()
{
    unsigned int total_alloced = 0;
    if (likely(q_manager.using) || likely(q_manager.freeList)) {
        printf("Readly Queue: ");
        for (BaseBlock *tmp = q_manager.freeList; tmp && ++total_alloced;
             tmp = tmp->next)
            printf("%p[%ld] -> ", tmp, tmp->size);

        printf("\nUsing Queue: ");
        for (BaseBlock *tmp = q_manager.using; tmp && ++total_alloced;
             tmp = tmp->next)
            printf("%p[%ld] -> ", tmp, tmp->size);
        printf("\n");
    } else {
        printf("The malloc space is empty now.\n");
    }
    printf("Total allocated: %u\n", total_alloced);
}