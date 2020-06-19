#include "test.h"

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

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

#define MAX(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b;      \
    })

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

            /* check answer */
            result += (curr->data == i++);
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

/*
 * Based on OOM-killer to allocate the largest capacity object.
 * Or your disk(swap) size is enough to store pages.
 *
 * @return: the lacated object size.
 *
 * This function will fork a child process to test mymalloc a large object, if
 * the memory is full or swap space is full, OS will kill it(child process)
 * automatically.
 *
 * Before OS kill this child process, the child process keeps sending the
 * locating size(not "located", actually the located memory will be freed by
 * myfree) to parent process. When the child process be killed, the pipeline
 * will be closed by OS, then the parent process will receive the EOF of
 * pipeline. Then sent the size back to the caller.
 *
 * Reference:
 * * OOM-killer:
 * https://www.kernel.org/doc/gorman/html/understand/understand016.html
 */
size_t testLargeObj(int viewTesting)
{
    int pipeFd[2];
    if (pipe(pipeFd) < 0) {
        perror("Pipe error!");
        return -1;
    }

    pid_t pid = fork();
    /* Is child */
    if (pid == 0) {
        /* Init IPC */
        close(pipeFd[0]);

        static const char *largeStringBasis = "THE_LARGE_STRING_BASIS.";
        int level = 0;

        /* Copy large string to str in first time */
        char *str = mycalloc(strlen(largeStringBasis), sizeof(char));
        TESTALLOCSUCCESS(str);
        strncpy(str, largeStringBasis, strlen(largeStringBasis));

        /*
         * In using object will up to 8.2TB.
         * Actually, it's 824633720832 Bytes.
         */
        while (level++ < 35) {
            /* Send current progress rate to parent */
            size_t len = strlen(str);
            ssize_t sendSize = write(pipeFd[1], &len, sizeof(size_t));
            if (!sendSize)
                perror("Broken pipe: ");

            /* Double the string */
            char *doubleStr = mymalloc((strlen(str) << 1) + 1);

            TESTALLOCSUCCESS(doubleStr);

            /* Concatenate the 'largeStringBasis' behind of 'str'*/
            strncpy(doubleStr, str, strlen(str));
            strncat(doubleStr, str, strlen(str));

            swap((void *) &doubleStr, (void *) &str);
            myfree(doubleStr);
        }

        if (viewTesting)
            printf("%s\n", str);

        /* Free alloced */
        myfree(str);

        /* End of IPC */
        close(pipeFd[1]);
        _exit(0);
    } else if (pid != -1) {
        size_t allocatedSize, tmp = 0;
        close(pipeFd[1]);
        while (read(pipeFd[0], &allocatedSize, sizeof(size_t)) > 0)
            allocatedSize = MAX(tmp, allocatedSize);
        return allocatedSize;
    } else {
        perror("fork failed");
        return -2; /* fork failed */
    }
}

void testAll(int viewTesting)
{
    /* Designated Initializer in C99, but not implemented in GNU C++ */
    struct Result {
        char *topic;
        size_t value;
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
        printf("%10s:\t%ld\n", result[i].topic, result[i].value);
}
