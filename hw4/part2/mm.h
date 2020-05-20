#ifndef __MY_MM_H_INCLUDED__
#define __MY_MM_H_INCLUDED__

/* A macro that in GCC to optimize for branch prediction */
#define likely(x) __builtin_expect(!!(x), 1)
/* Well, move unlikely while non-nesserary */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *mymalloc(size_t size);
void myfree(void *ptr);
void *myrealloc(void *ptr, size_t size);
void *mycalloc(size_t nmemb, size_t size);

/* To prevent memory leak */
void cleanAll();

/* Graphical printing mymalloc linking state */
void printMallocSpace();

#endif
