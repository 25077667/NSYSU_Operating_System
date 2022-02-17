#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

/* get input with dynamic length */
static char *inputString()
{
    size_t len = 0, size = 64;
    char *str = realloc(NULL, size), ch;
    if (!str)
        return NULL;
    while ((ch = fgetc(stdin)) != EOF && ch != '\n' && ch != '\r') {
        str[len++] = ch;
        if (len == size && !(str = realloc(str, (size += 64))))
            return NULL;
    }
    str[len++] = 0;
    /* If it's empty string free it and return NULL, else return the str */
    return (strlen((str = realloc(str, len))) ? str : ((free(str)), NULL));
}

static void do_exec(char **commands, size_t index)
{
    int fd[2], redirectFailed, status;
    pid_t pid;
    /* Fork a child to execute commands */
    if ((pid = fork())) { /* This is parent(aka. ./myshell) */
        void *notWait = strchr(commands[index - 1], '&');
        while (!notWait && waitpid(pid, &status, WUNTRACED | WCONTINUED) &&
               !WIFEXITED(status) && !WIFSIGNALED(status))
            ;
        (notWait) ? printf("[%d]\n", pid) : (void) "trash";
        return;
    }
    /* All commands would be executed here
     *
     * For example:
     * $ ls | cat | cat
     * Works like:
     *
     * .|   out          in   out          in   out          in
     * /|     ____________      ____________      ____________
     * m|    |            |    |            |    |            |
     * y|    |            |    |            |    |            |
     * s|----|     cat    |--->|     cat    |--->|     ls     |
     * h|    |            |    |            |    |            |
     * e|    |____________|    |____________|    |____________|
     * l|              ------------>     ------------>
     * l|                  fork              fork
     *
     *
     * Child's stdout to parent's stdin.
     *
     * And afterwards, kill all forked childs(commands).
     */
    while (--index) {
        /* set all all pipe line */
        if (pipe(fd) < 0)
            handle_error("Create pipe failed");
        pid = fork();
        if (pid > 0) {
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            /* Last be piped process will leave first */
            break;
        } else if (pid == 0) {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
        } else
            handle_error("Fork failed");
    }
    char *src = getFilename(commands[index], '<');
    char *dst = getFilename(commands[index], '>');
    if ((redirectFailed = redirect(src, dst)))
        handle_error((redirectFailed == -1)
                         ? "dup2 source failed"
                         : ((redirectFailed == -2) ? "dup2 destinate failed"
                                                   : "dup in/out both failed"));
    char *arg[] = {getArgs(commands[index], 0), getArgs(commands[index], 1),
                   NULL};
    (execvp(arg[0], arg) < 0) ? puts("Command not found!") : (void) "trash";

    /* If it's NULL ptr free another ptr.
     *
     * About the malloc(0).
     * The C standard (C17 7.22.3/1) says:
     * If the size of the space requested is zero, the behavior is
     * implementation defined: either a null pointer is returned, or the
     * behavior is as if the size were some nonzero value, except that the
     * returned pointer shall not be used to access an object.
     *
     * TL;DR:
     * ```
     * If size is 0, either a null pointer or a unique pointer that can be
     * successfully passed to free() shall be returned.[1]
     * ```
     * [1] https://pubs.opengroup.org/onlinepubs/009695399/functions/malloc.html
     */
    free(src ? src : malloc(0));
    free(dst ? dst : malloc(0));
    free(arg[0] ? arg[0] : malloc(0));
    free(arg[1] ? arg[1] : malloc(0));
    /* Kill all child process */
    exit(0);
}

int main()
{
    int keeping = 1;
    char *input = NULL, **commands;
    while (keeping) {
        size_t command_num;
        while (printf("$ "), input = inputString(), !input)
            ;
        commands = commandParser(input, &command_num);
        if (commands == (void *) -1)
            handle_error("mmap failed");
        /*Only "quit" or 'q' to exit, rather than exit */
        keeping = strcmp(input, "quit") && strcmp(input, "q");
        (keeping) ? do_exec(commands, command_num), "trash" : "trash";
        freeCommands(commands, command_num);
        free(input);
    }
    return 0;
}