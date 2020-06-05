#include "read.h"
#include "write.h"

int main()
{
    pid_t pid = fork();
    if(pid){
        testWrite("foo", "junk");
    }
    else{
        char buf[sysconf(_SC_PAGESIZE)];
        testRead("foo", buf);
        printf("READ: %s", buf);
    }
    return 0;
}