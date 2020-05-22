#ifndef TEST_H
#define TEST_H
#include <stddef.h>

typedef struct obj {
    struct obj *next;
    int data;
} Obj;

typedef struct list {
    Obj *head, *tail;
    int size;
} List;

void testAll(int viewTesting);

/* Test cases*/
int testStruct(int viewTesting);
int testInt(int viewTesting);
int testBool(int viewTesting);
int testPtr(int viewTesting);
int testString(int viewTesting);

/* The full of memory case */
int testLargeObj(int viewTesting);
#endif