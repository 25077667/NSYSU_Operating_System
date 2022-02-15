#include "parser.h"

/* Calculate the number of columns */
static inline size_t getColumns(const char *input)
{
    size_t columns = 1;
    for (int i = 0; input[i]; i++)
        columns += input[i] == '|';
    return columns;
}

char **commandParser(const char *input, size_t *columns)
{
    size_t input_len = strlen(input);
    (*columns) = getColumns(input);

    char **commands = (char **) malloc(sizeof(char *) * (*columns));
    char *copied = calloc(input_len + 1, sizeof(char));
    char *prev_cut = copied;
    strncpy(copied, input, input_len);

    /* Split command to commands, aka. 2D array */
    for (int i = 0; i < (*columns) && (*prev_cut); i++) {
        while (isspace(*prev_cut))
            prev_cut++;
        char *pipe_pos = strchr(prev_cut, '|');

        /* Cut and paste to commands buffer */
        size_t cut_len = ((pipe_pos - prev_cut) == -(long long) prev_cut)
                             ? strlen(prev_cut)
                             : pipe_pos - prev_cut;
        commands[i] = calloc(cut_len + 1, sizeof(char));
        if (commands[i] == (void *) -1)
            handle_error("Allocate sharing memory failed");
        memcpy(commands[i], prev_cut, cut_len);

        prev_cut += cut_len + 1;  // skip the pipe
    }
    free(copied);
    return commands;
}

void freeCommands(char **commands, size_t column)
{
    for (int i = 0; i < column; i++)
        free(commands[i]);
    free(commands);
}

int redirect(const char *src, const char *dst)
{
    FILE *_in = src ? fopen(src, "r") : stdin;
    FILE *_out = dst ? fopen(dst, "w") : stdout;
    return (dup2(fileno(_in), STDIN_FILENO) == -1) * (-1) +
           (dup2(fileno(_out), STDOUT_FILENO) == -1) * (-2);
}

char *getFilename(const char *command, char symbol)
{
    /* strlen is an O(N) function, put doRecord here to reduce one line */
    size_t len = strlen(command), j = 0, doRecord = 0;
    char *filename = calloc(len, sizeof(char));
    /* get substr begin and end */
    for (size_t i = 0; i < len; i++) {
        i += (command[i] == '\\' && doRecord);
        if (doRecord && (!isspace(command[i]) || command[i - 1] == '\\'))
            filename[j++] = command[i];

        /*
         * What is the (symbol ^ 2)?
         * Well, the symbol is only 2 status('<' and '>')
         * And view them by the ascii, just "toggle the second bit" is the other
         * symbol.
         */
        doRecord = doRecord ? (command[i] != (symbol ^ 2) && command[i + 1])
                            : (command[i] == symbol);
    }
    /* Realloc to fit the string, setting the last byte to '\0', then
     * if the string is empty string return NULL, or get the filename */
    return (filename = realloc(filename, j + 1), filename[j] = '\0',
            (j ? filename : (free(filename), NULL)));
}

char *getArgs(const char *command, int option)
{
    size_t len = strlen(command) + 1, pureCmd = 0, diameter = 0;
    /* Before the seperator( '>' '<' '&' ';' ) is the pure command with args */
    for (int i = 0; i < len && command[i] != '<' && command[i] != '>' &&
                    command[i] != '&' && command[i] != ';';
         i++)
        pureCmd = i;
    /* Remove extea <space> before the seperator */
    while (!isalnum(command[pureCmd - 1]) && !isalnum(command[pureCmd]))
        pureCmd--;
    /* The diameter is the "cut point" between the command and args*/
    while (diameter < pureCmd && isalnum(command[diameter]))
        diameter++;

    /* Allocate suitable space, malloc and set '\0' would take 2 lines. But
     * calloc take 1 line */
    char *result = calloc((option) ? (pureCmd - diameter + 1) : diameter + 1,
                          sizeof(char));
    memcpy(result, command + ((option) ? diameter + 1 : 0),
           (option) ? (pureCmd - diameter) : diameter);
    /* If it's empty string free it and return NULL, else return the origin */
    return (strlen(result) ? (result) : (free(result), NULL));
}