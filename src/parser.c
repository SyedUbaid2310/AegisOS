#include <string.h>

#include "parser.h"

int parse_command(char *input, char *args[])
{
    int argc = 0;

    char *token = strtok(input, " \t");

    while (token != NULL && argc < MAX_ARGS - 1)
    {
        args[argc] = token;
        argc++;

        token = strtok(NULL, " \t");
    }

    args[argc] = NULL;

    return argc;
}
