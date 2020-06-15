#include "read.h"
#include "write.h"

/**
 * Reference:
 * https://courses.cs.washington.edu/courses/cse451/06au/slides/mmap-copy.c
 * https://stackoverflow.com/questions/26259421/use-mmap-in-c-to-write-into-memory
 * https://man7.org/linux/man-pages/man2/mmap.2.html
 */

int main()
{
    /**
     * Generate 2 process to communication with mmap,
     * each process map the "foo" file into their memory space.
     *
     * Yeah, it's immediately.
     * Add sleep(10) in "write.c" to sleep for 10 seconds.
     * (You can see that "Before munmap() the file, the child get the context
     * writen in.")
     */
    pid_t pid = fork();
    if (pid) {
        testWrite("foo", "junk\n");
        waitpid(pid, NULL, 0);
    } else {
        char buf[sysconf(_SC_PAGESIZE)];
        testRead("foo", buf);
        printf("READ: %s", buf);
    }
    return 0;
}
