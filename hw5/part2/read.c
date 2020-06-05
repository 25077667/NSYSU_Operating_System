#include "read.h"

void testRead(const char *filename, void *buf)
{
    int fd;
    struct stat sb;

    fd = open(filename, O_RDONLY);
    /* Bitwise operation to identify is -1 */
    if (!(~fd))
        handle_error("Open");

    /* To obtain file size */
    if (fstat(fd, &sb) == -1)
        handle_error("fstat");

    void *addr = mmap(NULL, getpagesize(), PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        handle_error("mmap");

    /* Make sure it's up to date before copy */
    msync(addr, getpagesize(), MS_SYNC);
    memcpy(buf, addr, getpagesize());

    munmap(addr, getpagesize());
    close(fd);
}