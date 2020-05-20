#include "test.h"

#include <stdbool.h>
#include <stddef.h>

#include "mm.h"
#define testTypes 5

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
    Obj *obj = (Obj *) mymalloc(sizeof(Obj));
    List *list = (List *) mymalloc(sizeof(List));
    list->head = list->tail = NULL;
    list->size = 0;

    for (i = 0; i < 100; i++) {
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

    for (i = 0; i < 100; i++)
        pop_front(list);

    return result;
}

void testAll(int viewTesting)
{
    struct Result {
        char *topic;
        int value;
    } result[testTypes] = {
        {.topic = "Struct", .value = 0}, {.topic = "Struct", .value = 0},
        {.topic = "Struct", .value = 0}, {.topic = "Struct", .value = 0},
        {.topic = "Struct", .value = 0},
    };

    result[0].value = testStruct(viewTesting);
    printf("%d\n", result[0].value);
}
