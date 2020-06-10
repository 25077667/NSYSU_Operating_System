#include "write.h"

void testWrite(const char *filename, const void *context)
{
    int fd;
    void *dst;

    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 00777);
    if (fd == -1)
        handle_error("write open");

    /*
     * Move FD's file position to OFFSET bytes from the beginning of the file.
     * And return the new file position.(ignaore here)
     */
    lseek(fd, strlen(context), SEEK_SET);

    /* write a dummy byte at the last location */
    if (write(fd, "", 1) != 1)
        handle_error("write error");

    dst = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (dst == MAP_FAILED)
        handle_error("mmap");

    memcpy(dst, context, strlen(context));

    munmap(dst, getpagesize());
    close(fd);
}
