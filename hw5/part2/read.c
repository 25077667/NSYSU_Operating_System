#include "read.h"

void testRead(const char *filename, void *buf)
{
    int fd;

    fd = open(filename, O_RDONLY);
    /* Bitwise operation to identify is -1 */
    if (!(~fd))
        handle_error("read open");

    void *addr = mmap(NULL, getpagesize(), PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        handle_error("mmap");


    memcpy(buf, addr, getpagesize());
    msync(addr, getpagesize(), MS_SYNC);

    munmap(addr, getpagesize());
    close(fd);
}
