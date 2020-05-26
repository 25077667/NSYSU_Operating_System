#include "test.h"

#include <signal.h>

#include "mm.h"
#define TEST_TYPES 6
#define TEST_TIMES 10

#define TESTALLOCSUCCESS(x)                      \
    do {                                         \
        if (!x) {                                \
            fprintf(stderr, "Memory is full\n"); \
            exit(0);                             \
        }                                        \
    } while (0);

#define FOR_TEST for (int index = 0; index < TEST_TIMES; index++)



static void push_back(List *l, Obj *o)
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

static void pop_front(List *l)
{
    if (!l || !l->head)
        return;
    Obj *tmp = l->head;
    l->head = l->head->next;
    myfree(tmp);
    l->size--;
}

static void swap(void **a, void **b)
{
    void *tmp = *a;
    *a = *b;
    *b = tmp;
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
    FOR_TEST
    {
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

        for (i = 0; i < 10; i++)
            pop_front(list);
    }

    return result;
}

int testInt(int viewTesting)
{
    int result = 0;
    FOR_TEST
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

    return result;
}

int testBool(int viewTesting)
{
    int result = 0;
    FOR_TEST
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

    return result;
}

int testPtr(int viewTesting)
{
    int result = 0;
    FOR_TEST
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

    return result;
}

int testString(int viewTesting)
{
    static const char *test_str = "THIS_IS_A_STRING.";
    int result = 0;
    FOR_TEST
    for (int i = 0; i < 10; i++) {
        char *str = mycalloc(strlen(test_str) + 1, sizeof(char));

        TESTALLOCSUCCESS(str);

        /* Test for can R/W the block */
        strncpy(str, test_str, strlen(test_str));
        if (strcmp(str, test_str) == 0)
            result++;

        if (viewTesting)
            printf("%s %s\n", str, test_str);

        /* Free alloced */
        myfree(str);
    }

    return result;
}

int testLargeObj(int viewTesting)
{
    static const char *largeStringBasis = "THE_LARGE_STRING_BASIS.";
    int result = 0;

    /* Copy large string to str in first time */
    char *str = mycalloc(strlen(largeStringBasis), sizeof(char));
    TESTALLOCSUCCESS(str);
    strncpy(str, largeStringBasis, strlen(largeStringBasis));

    /*
     * In using object will up to 12GB.
     * But ready(freed) object will up to 12GB, too.
     * That is you can optimize the `myfree()` to save more memory.
     */
    while (result < 29) {
        /* Double the string */
        char *doubleStr = mymalloc((strlen(str) << 1) + 1);

        TESTALLOCSUCCESS(doubleStr);

        /* Concatenate the 'largeStringBasis' behind of 'str'*/
        strncpy(doubleStr, str, strlen(str));
        strncat(doubleStr, str, strlen(str));

        swap((void *) &doubleStr, (void *) &str);
        result++;
        myfree(doubleStr);
    }

    if (viewTesting)
        printf("%s\n", str);

    /* Free alloced */
    myfree(str);

    return result;
}

void testAll(int viewTesting)
{
    struct Result {
        char *topic;
        int value;
    } result[TEST_TYPES] = {
        {.topic = "struct", .value = 0}, {.topic = "int", .value = 0},
        {.topic = "bool", .value = 0},   {.topic = "pointer", .value = 0},
        {.topic = "string", .value = 0}, {.topic = "Large", .value = 0},
    };

    result[0].value = testStruct(viewTesting);
    result[1].value = testInt(viewTesting);
    result[2].value = testBool(viewTesting);
    result[3].value = testPtr(viewTesting);
    result[4].value = testString(viewTesting);
    result[5].value = testLargeObj(viewTesting);

    for (int i = 0; i < TEST_TYPES; i++)
        printf("%10s:\t%d\n", result[i].topic, result[i].value);
    printf("Test Finlished!\n");
    printf("\nTotal used spaces:\n");
    printMallocSpace();
}
