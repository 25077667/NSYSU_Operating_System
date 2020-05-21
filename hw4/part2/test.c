#include "test.h"

#include <stdbool.h>
#include <stdio.h>

#include "mm.h"
#define testTypes 4

#define TESTALLOCSUCCESS(x)                      \
    do {                                         \
        if (!x) {                                \
            fprintf(stderr, "Memory is full\n"); \
            exit(0);                             \
        }                                        \
    } while (0);

void push_back(List *l, Obj *o)
{
    if (!(l))
        return;

    Obj *newTail = (Obj *) mymalloc(sizeof(Obj));
    if (!(newTail))
        return;
    memcpy(newTail, o, sizeof(Obj));
    newTail->next = NULL;

    if (l->tail)
        l->tail->next = newTail;
    l->tail = newTail;
    if (!l->head)
        l->head = newTail;
    l->size++;
}

void pop_front(List *l)
{
    if (!l || !l->head)
        return;
    Obj *tmp = l->head;
    l->head = l->head->next;
    myfree(tmp);
    l->size--;
}

int testStruct(int viewTesting)
{
    int i;
    int result = 0;
    Obj *obj = mymalloc(sizeof(Obj));
    List *list = mymalloc(sizeof(List));

    TESTALLOCSUCCESS(obj);
    TESTALLOCSUCCESS(list);

    list->head = list->tail = NULL;
    list->size = 0;

    /* Test for allocating and free for 1000 times */
    for (int testTimes = 0; testTimes < 10; testTimes++) {
        for (i = 0; i < 10; i++) {
            obj->data = i;
            push_back(list, obj);
        }

        i = 0;
        for (Obj *curr = list->head; curr; curr = curr->next) {
            /* View test */
            if (viewTesting)
                printf("%d %d\n", curr->data, i);
            result += (curr->data == i++);
            /* check answer */
        }
        // printMallocSpace();
        for (i = 0; i < 10; i++)
            pop_front(list);
    }

    return result;
}

int testInt(int viewTesting)
{
    int result = 0;
    for (int testTimes = 0; testTimes < 10; testTimes++) {
        for (int i = 0; i < 10; i++) {
            bool *bool_ptr = mymalloc(sizeof(bool));
            int *int_ptr = myrealloc(bool_ptr, sizeof(int));

            TESTALLOCSUCCESS(int_ptr);

            /* Test for can R/W the block */
            (*int_ptr) = i;
            if ((*int_ptr) == i)
                result++;

            if (viewTesting)
                printf("%d %d\n", *int_ptr, i);

            /* Free alloced */
            myfree(int_ptr);
        }
    }
    return result;
}

int testBool(int viewTesting)
{
    int result = 0;
    for (int testTimes = 0; testTimes < 10; testTimes++) {
        for (int i = 0; i < 10; i++) {
            bool *bool_ptr = mycalloc(1, sizeof(bool));

            TESTALLOCSUCCESS(bool_ptr);

            /* Test for can R/W the block */
            (*bool_ptr) = i & 1;
            if ((*bool_ptr) == (i & 1))
                result++;

            if (viewTesting)
                printf("%d %d\n", *bool_ptr, i & 1);

            /* Free alloced */
            myfree(bool_ptr);
        }
    }
    return result;
}

int testPtr(int viewTesting)
{
    int result = 0;
    for (int testTimes = 0; testTimes < 10; testTimes++) {
        for (int i = 0; i < 10; i++) {
            void **void_ptr = mycalloc(1, sizeof(void *));

            TESTALLOCSUCCESS(void_ptr);

            /* Test for can R/W the block */
            (*void_ptr) = &i;
            if ((*void_ptr) == &i)
                result++;

            if (viewTesting)
                printf("%p %p\n", *void_ptr, &i);

            /* Free alloced */
            myfree(void_ptr);
        }
    }
    return result;
}

void testAll(int viewTesting)
{
    struct Result {
        char *topic;
        int value;
    } result[testTypes] = {
        {.topic = "struct", .value = 0},
        {.topic = "int", .value = 0},
        {.topic = "bool", .value = 0},
        {.topic = "pointer", .value = 0},
    };

    result[0].value = testStruct(viewTesting);
    result[1].value = testInt(viewTesting);
    result[2].value = testBool(viewTesting);
    result[3].value = testPtr(viewTesting);

    for (int i = 0; i < testTypes; i++)
        printf("%10s:\t%d\n", result[i].topic, result[i].value);
    printf("Test Finlished!\n");
    printf("\nTotal used spaces:\n");
    printMallocSpace();
}
