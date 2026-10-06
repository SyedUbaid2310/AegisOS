#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "parser.h"
#include "process.h"
#include "builtin.h"
#include "jobs.h"

#define INPUT_SIZE 1024

void shell_run(void)
{
    char input[INPUT_SIZE];
    char *args[MAX_ARGS];

    while (1)
    {
        cleanup_jobs();

        printf("AegisOS> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '\0')
        {
            continue;
        }

        int argc = parse_command(input, args);

        if (argc == 0)
        {
            continue;
        }

        if (strcmp(args[0], "jobs") == 0)
        {
            show_jobs();
            continue;
        }

        if (handle_builtin(args))
        {
            continue;
        }

        execute_command(args);
    }
}
