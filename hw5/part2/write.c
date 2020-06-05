#include "wirte.h"

void testWirte(const char *filename, const void *context)
{
    int fd;
    struct stat sb;
    void *dst;

    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0x0777);
    if (fd == -1)
        handle_error("Open");

    if (fstat(fd, &sb) == -1)
        handle_error("fstat");

    /* write a dummy byte at the last location */
    if (write(fd, "", 1) != 1)
        handle_error("write error");

    dst = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (dst == MAP_FAILED)
        handle_error("mmap");

    memcpy(dst, context, strlen(context) + 1);

    munmap(dst, 0);
    close(fd);
}
