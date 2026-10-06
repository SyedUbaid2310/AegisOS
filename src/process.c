#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "process.h"

void execute_command(char *args[])
{
    int background = 0;
    int i = 0;

    while (args[i] != NULL)
    {
        if (strcmp(args[i], "&") == 0)
        {
            background = 1;
            args[i] = NULL;
            break;
        }

        i++;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("AegisOS: fork");
        return;
    }

    if (pid == 0)
    {
        setpgid(0, 0);
        signal(SIGINT, SIG_DFL);

        i = 0;

        while (args[i] != NULL)
        {
            if (strcmp(args[i], ">") == 0)
            {
                if (args[i + 1] == NULL)
                {
                    fprintf(stderr,
                            "AegisOS: missing output filename\n");
                    exit(EXIT_FAILURE);
                }

                int file = open(
                    args[i + 1],
                    O_WRONLY | O_CREAT | O_TRUNC,
                    0644
                );

                if (file < 0)
                {
                    perror("AegisOS: open");
                    exit(EXIT_FAILURE);
                }

                dup2(file, STDOUT_FILENO);
                close(file);

                args[i] = NULL;
                break;
            }

            if (strcmp(args[i], "<") == 0)
            {
                if (args[i + 1] == NULL)
                {
                    fprintf(stderr,
                            "AegisOS: missing input filename\n");
                    exit(EXIT_FAILURE);
                }

                int file = open(args[i + 1], O_RDONLY);

                if (file < 0)
                {
                    perror("AegisOS: open");
                    exit(EXIT_FAILURE);
                }

                dup2(file, STDIN_FILENO);
                close(file);

                args[i] = NULL;
                break;
            }

            i++;
        }

        execvp(args[0], args);

        perror("AegisOS: execvp");
        exit(EXIT_FAILURE);
    }

    setpgid(pid, pid);

    if (background)
    {
        printf("[background process started: %d]\n", pid);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
}
