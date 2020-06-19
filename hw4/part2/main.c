#include "mm.h"
#include "test.h"

#define CHECK_Y(x) ((x) == 'y' || (x) == 'Y')

int main()
{
    char viewResult;
    printf("Do you want to see test result[Y/n]: ");
    viewResult = getchar();

    testAll(CHECK_Y(viewResult));
    printf("Test Finlished!\n");
    printf("\nTotal used spaces:\n");
    printMallocSpace();

    return 0;
}