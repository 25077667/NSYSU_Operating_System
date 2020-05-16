#include <stdio.h>

#include "mm.h"

typedef struct obj {
    struct obj *next;
    int data;
} Obj;

typedef struct list {
    Obj *head, *tail;
    int size;
} List;

void push_back(List *l, Obj *o)
{
    if (unlikely(l))
        return;

    Obj *newTail = mymalloc(sizeof(o));
    if (unlikely(newTail))
        return;
    memcpy(newTail, o, sizeof(*o));

    if (l->tail)
        l->tail->next = newTail;
    l->tail = newTail;
    if (!l->head)
        l->head = newTail;
    l->size++;
}

void pop_front(List *l)
{
    if (unlikely(l) || unlikely(l->head))
        return;
    Obj *tmp = l->head;
    l->head = l->head->next;
    myfree(tmp);
    l->size--;
}

int main()
{
    Obj *obj = mymalloc(sizeof(Obj));
    List *list = mymalloc(sizeof(List));

    printf("%lu %lu\n", obj->data, list);

    for (int i = 0; i < 100; i++) {
        obj->data = i;
        push_back(list, obj);
    }
    for (Obj *curr = list->head; curr; curr = curr->next)
        printf("%d ", curr->data);

    for (int i = 0; i < 100; i++)
        pop_front(list);

    cleanAll();
    return 0;
}