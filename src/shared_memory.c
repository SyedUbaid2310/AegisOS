#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "shared_memory.h"

#define SHARED_MEMORY_NAME "/aegisos_shared_memory"
#define SHARED_MEMORY_SIZE 256

void run_shared_memory_demo(void)
{
    int shm_fd;
    pid_t child_pid;

    const char message[] =
        "Hello from AegisOS shared memory!";

    printf("AegisOS: Starting shared memory IPC demonstration\n");

    /*
     * Create a POSIX shared-memory object.
     */
    shm_fd = shm_open(
        SHARED_MEMORY_NAME,
        O_CREAT | O_RDWR,
        0666
    );

    if (shm_fd == -1)
    {
        perror("AegisOS: shm_open");
        return;
    }

    /*
     * Set the size of the shared-memory object.
     */
    if (ftruncate(shm_fd, SHARED_MEMORY_SIZE) == -1)
    {
        perror("AegisOS: ftruncate");

        close(shm_fd);
        shm_unlink(SHARED_MEMORY_NAME);

        return;
    }

    /*
     * Map shared memory into the process address space.
     */
    char *shared_memory = mmap(
        NULL,
        SHARED_MEMORY_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );

    if (shared_memory == MAP_FAILED)
    {
        perror("AegisOS: mmap");

        close(shm_fd);
        shm_unlink(SHARED_MEMORY_NAME);

        return;
    }

    printf("AegisOS: Shared memory created successfully\n");

    /*
     * The file descriptor is no longer needed after mmap().
     */
    close(shm_fd);

    child_pid = fork();

    if (child_pid == -1)
    {
        perror("AegisOS: fork");

        munmap(shared_memory, SHARED_MEMORY_SIZE);
        shm_unlink(SHARED_MEMORY_NAME);

        return;
    }

    /*
     * =========================
     * CHILD PROCESS
     * =========================
     */
    if (child_pid == 0)
    {
        printf("AegisOS: Child reading shared memory...\n");

        printf(
            "AegisOS: Child received: %s\n",
            shared_memory
        );

        munmap(shared_memory, SHARED_MEMORY_SIZE);

        printf("AegisOS: Child process finished\n");

        exit(EXIT_SUCCESS);
    }

    /*
     * =========================
     * PARENT PROCESS
     * =========================
     */

    printf("AegisOS: Parent writing to shared memory...\n");

    /*
     * Clear the shared-memory region before writing.
     */
    memset(shared_memory, 0, SHARED_MEMORY_SIZE);

    strncpy(
        shared_memory,
        message,
        SHARED_MEMORY_SIZE - 1
    );

    shared_memory[SHARED_MEMORY_SIZE - 1] = '\0';

    waitpid(child_pid, NULL, 0);

    printf("AegisOS: Parent detected child completion\n");

    munmap(shared_memory, SHARED_MEMORY_SIZE);

    /*
     * Remove the shared-memory object.
     */
    shm_unlink(SHARED_MEMORY_NAME);

    printf("AegisOS: Shared memory IPC demonstration completed\n");
}
