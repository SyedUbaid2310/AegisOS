#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#include "builtin.h"
#include "threads.h"
#include "ipc.h"
#include "shared_memory.h"
#include "sync.h"
#include "condition.h"
#include "message_queue.h"
#include "memory.h"
#include "virtual_memory.h"
#include "process_monitor.h"
#include "system_info.h"
#include "filesystem.h"
#include "diagnostics.h"
#include "help.h"
#include "inspect.h"

int handle_builtin(char *args[])
{
    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            fprintf(
                stderr,
                "AegisOS: cd: missing argument\n"
            );
        }
        else if (chdir(args[1]) != 0)
        {
            perror("AegisOS: cd");
        }

        return 1;
    }

    if (strcmp(args[0], "pwd") == 0)
    {
        char cwd[PATH_MAX];

        if (getcwd(
                cwd,
                sizeof(cwd)) != NULL)
        {
            printf(
                "%s\n",
                cwd
            );
        }
        else
        {
            perror("AegisOS: pwd");
        }

        return 1;
    }

    if (strcmp(args[0], "threads") == 0)
    {
        run_thread_demo();
        return 1;
    }

    if (strcmp(args[0], "ipc") == 0)
    {
        run_ipc_demo();
        return 1;
    }

    if (strcmp(args[0], "shm") == 0)
    {
        run_shared_memory_demo();
        return 1;
    }

    if (strcmp(args[0], "sync") == 0)
    {
        run_sync_demo();
        return 1;
    }

    if (strcmp(args[0], "cond") == 0)
    {
        run_condition_demo();
        return 1;
    }

    if (strcmp(args[0], "mq") == 0)
    {
        run_message_queue_demo();
        return 1;
    }

    if (strcmp(args[0], "memory") == 0)
    {
        run_memory_demo();
        return 1;
    }

    if (strcmp(args[0], "mmap") == 0)
    {
        run_virtual_memory_demo();
        return 1;
    }

    if (strcmp(args[0], "monitor") == 0)
    {
        run_process_monitor();
        return 1;
    }

    if (strcmp(args[0], "sysinfo") == 0)
    {
        run_system_info();
        return 1;
    }

    if (strcmp(args[0], "fs") == 0)
    {
        run_filesystem_demo();
        return 1;
    }

    if (strcmp(args[0], "diag") == 0)
    {
        run_diagnostics_demo();
        return 1;
    }

    if (strcmp(args[0], "help") == 0)
    {
        run_help();
        return 1;
    }

    if (strcmp(args[0], "inspect") == 0)
    {
        if (args[1] == NULL)
        {
            printf(
                "AegisOS: Usage: inspect <PID>\n"
            );
        }
        else
        {
            run_process_inspector(args[1]);
        }

        return 1;
    }

    if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }

    return 0;
}
