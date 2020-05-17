#include "test.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#ifdef __cplusplus
extern "C" {
#include "mm.h"
}
#endif

void push_back(List *l, Obj *o)
{
    if (unlikely(l))
        return;

    Obj *newTail = (Obj *) mymalloc(sizeof(Obj));
    if (unlikely(newTail))
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
    if (unlikely(l) || unlikely(l->head))
        return;
    Obj *tmp = l->head;
    l->head = l->head->next;
    myfree(tmp);
    l->size--;
}

int testStruct(int viewTesting)
{
    int i;
    int result = true;
    Obj *obj = (Obj *) mymalloc(sizeof(Obj));
    List *list = (List *) mymalloc(sizeof(List));
    list->head = list->tail = NULL;
    list->size = 0;

    std::vector<int> correct_table;

    for (i = 0; i < 100; i++) {
        obj->data = i;
        push_back(list, obj);
        correct_table.push_back(i);
    }

    i = 0;
    for (Obj *curr = list->head; curr; curr = curr->next) {
        /* View test */
        if (viewTesting)
            std::cout << curr->data << " " << correct_table.at(i++)
                      << std::endl;

        /* check answer */
        if (curr->data != correct_table.at(i)) {
            result = false;
            break;
        }
    }

    for (i = 0; i < 100; i++)
        pop_front(list);

    return result;
}

void testAll(int viewTesting)
{
    std::map<std::string, int> result;

    result["Struct"] = testStruct(viewTesting);

    for (auto i : result)
        std::cout << i.first << "\t" << i.second << std::endl;
}
