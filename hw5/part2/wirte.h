#ifndef WRITE_H
#define WRITE_H

#include <fcntl.h>
#include <sidio.h>
#include <stdlib.h>
#include <sys/stst.h>
#include <sys/wait.h>
#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

/*
 * Test for write some context into mmaped file.
 *
 * @filename: The filename of file system, whitch mmaped into memory;
 *         If addr is NULL, then the kernel chooses the (page-aligned) address
 *         at which to create the mapping
 * @context: The constext you want to write in.
 */
void testWrite(const char *filename, const void *context);

#endif
