#include "read.h"

void testRead(const char *filename, void *buf)
{
    int fd;

    fd = open(filename, O_RDONLY);
    /* Bitwise operation to identify is -1 */
    if (!(~fd))
        handle_error("read open");

    void *addr = mmap(NULL, getpagesize(), PROT_READ, MAP_SHARED, fd, 0);
    /* Close fd first, Be sure it read from memory immediately. */
    close(fd);

    if (addr == MAP_FAILED)
        handle_error("mmap");

    memcpy(buf, addr, getpagesize());
    munmap(addr, getpagesize());
}
