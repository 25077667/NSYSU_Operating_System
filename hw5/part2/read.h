#ifndef READ_H
#define READ_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

/*
 * To test mmapping a file.
 *
 * @filename: the file you want to mmap and read as char*
 * @buf: The buffer you provide to contain readed in data.
 */
void testRead(const char *filename, void *buf);

#endif