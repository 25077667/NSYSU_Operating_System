#ifndef __MY_MM_H_INCLUDED__
#define __MY_MM_H_INCLUDED__

/* A macro that in GCC to optimize for branch prediction */
#define likely(x) __builtin_expect(!!(x), 1)

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);
void *mycalloc(size_t nmemb, size_t size);

/*
 * `sbrk()` will auto free, while the process exit
 * Might freed by `_start()`, actually I don't know how it be freed.
 */

/* Graphical printing mymalloc linking state */
void printMallocSpace();

#endif
