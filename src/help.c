#include <stdio.h>

#include "help.h"

void run_help(void)
{
    printf("\n");
    printf("============================================================\n");
    printf("                    AegisOS Help System                    \n");
    printf("============================================================\n");

    printf("\n");
    printf("Basic Shell Commands\n");
    printf("------------------------------------------------------------\n");

    printf("  cd <directory>   Change the current working directory\n");
    printf("  pwd              Display the current working directory\n");
    printf("  jobs             Display background jobs\n");
    printf("  inspect <PID>    Inspect a running process\n");
    printf("  exit             Exit AegisOS\n");
    printf("  help             Display this help information\n");

    printf("\n");
    printf("Operating System Demonstrations\n");
    printf("------------------------------------------------------------\n");

    printf("  threads          POSIX threads demonstration\n");
    printf("  ipc              Bidirectional pipe IPC demonstration\n");
    printf("  shm              POSIX shared-memory demonstration\n");
    printf("  sync             Semaphore synchronization demonstration\n");
    printf("  cond             Condition-variable producer/consumer demo\n");
    printf("  mq               POSIX message-queue demonstration\n");

    printf("\n");
    printf("Memory and Process Management\n");
    printf("------------------------------------------------------------\n");

    printf("  memory           Dynamic memory allocation demonstration\n");
    printf("  mmap             Virtual memory / mmap demonstration\n");
    printf("  monitor          Process monitoring using /proc\n");
    printf("  inspect <PID>    Unified process, CPU, memory and FD inspector\n");
    printf("  sysinfo          CPU and system memory information\n");

    printf("\n");
    printf("File System and Diagnostics\n");
    printf("------------------------------------------------------------\n");

    printf("  fs               File-system operations demonstration\n");
    printf("  diag             Error handling and diagnostics demonstration\n");

    printf("\n");
    printf("Process Inspector\n");
    printf("------------------------------------------------------------\n");

    printf("  inspect <PID>    Inspect a running Linux process\n");
    printf("\n");
    printf("  The inspector reports:\n");
    printf("    * Process ID and parent process ID\n");
    printf("    * Process state and thread count\n");
    printf("    * Executable path\n");
    printf("    * CPU usage and CPU time\n");
    printf("    * CPU model and logical CPU count\n");
    printf("    * Virtual, resident, data and stack memory\n");
    printf("    * System memory information\n");
    printf("    * Open file descriptors and their targets\n");

    printf("\n");
    printf("External Commands\n");
    printf("------------------------------------------------------------\n");

    printf("  Any command not listed above is treated as an external\n");
    printf("  program and executed as a child process when available.\n");

    printf("\n");
    printf("AegisOS demonstrates concepts including:\n");
    printf("  * Process creation and execution\n");
    printf("  * Background processes and job control\n");
    printf("  * Signals\n");
    printf("  * Pipes and IPC\n");
    printf("  * POSIX threads\n");
    printf("  * Synchronization primitives\n");
    printf("  * Shared memory\n");
    printf("  * POSIX message queues\n");
    printf("  * Dynamic and virtual memory\n");
    printf("  * Process monitoring\n");
    printf("  * Live process inspection\n");
    printf("  * CPU and memory information\n");
    printf("  * File descriptor inspection\n");
    printf("  * File-system operations\n");
    printf("  * Error handling and diagnostics\n");

    printf("\n");
    printf("============================================================\n");
    printf("                 End of AegisOS Help                       \n");
    printf("============================================================\n");
    printf("\n");
}
