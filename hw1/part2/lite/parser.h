#ifndef PARSER_H
#define PARSER_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define handle_error(s) \
    do {                \
        perror((s));    \
        _exit(1);       \
    } while (0)

/*
 * A Parser that seperate commands into 2D-array by pipe line
 * @column: get the column size
 */
char **commandParser(const char *input, size_t *column);
void freeCommands(char **commands, size_t column);

/* redirect from/to a file, if filename is NULL that is from the stdIO
 * return 0 if success, -1 src failed, -2 dst failed, -3 both.
 */
int redirect(const char *src, const char *dst);

/* Get file name with realloc
 * @symbol: the direction symbol(aka. '<' and '>')
 * @Return: return NULL if not found.
 * return new allocate string if be found. You should free it by yourself.
 */
char *getFilename(const char *command, char symbol);

/* Get command arguements
 * @option: command name if 0, other arguements if 1.
 */
char *getArgs(const char *command, int option);
#endif