#include <dlfcn.h>
#include <stdio.h>

#include "mm.h"

#define CHECK_Y(x) ((x) == 'y' || (x) == 'Y')

int main()
{
    /* Prepare share library */
    void *testLib = dlopen("libtest.so", RTLD_LAZY);
    if (!testLib) {
        fputs(dlerror(), stderr);
        exit(1);
    }
    void (*testAll)(int);

    /* Clean the allocated spaces */
    char viewResult;
    printf("Do you want to see test result[Y/n]: ");

    do {
    } while (scanf("%c", &viewResult) == 0);

    testAll = (void (*)()) dlsym(testLib, "testAll");

    if (testAll)
        testAll(CHECK_Y(viewResult));

    dlclose(testLib);
    cleanAll();
    return 0;
}