#ifndef TEST_HPP
#define TEST_HPP
#include <stddef.h>

#include "mm.h"

typedef struct obj {
    struct obj *next;
    int data;
} Obj;

typedef struct list {
    Obj *head, *tail;
    int size;
} List;

void push_back(List *l, Obj *o);

void pop_front(List *l);

void testAll(int viewTesting);

/* Test cases*/
int testStruct(int viewTesting);
int testClass(int viewTesting);
int testString(int viewTesting);

/* The full of memory case */
int testLargeObj(int viewTesting);
#endif