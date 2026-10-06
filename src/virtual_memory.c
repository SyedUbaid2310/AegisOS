#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "virtual_memory.h"

#define MMAP_FILE "aegis_mmap_demo.txt"
#define MMAP_SIZE 256

void run_virtual_memory_demo(void)
{
    int file_descriptor;
    char *mapped_memory;

    const char initial_message[] =
        "AegisOS: Original data stored in mapped file.";

    const char updated_message[] =
        "AegisOS: Data modified through virtual memory mapping.";

    printf(
        "AegisOS: Starting virtual memory demonstration\n"
    );

    printf(
        "AegisOS: Creating memory-mapped file: %s\n",
        MMAP_FILE
    );

    file_descriptor = open(
        MMAP_FILE,
        O_RDWR | O_CREAT | O_TRUNC,
        0666
    );

    if (file_descriptor == -1)
    {
        perror("AegisOS: open");
        return;
    }

    if (ftruncate(file_descriptor, MMAP_SIZE) == -1)
    {
        perror("AegisOS: ftruncate");

        close(file_descriptor);
        unlink(MMAP_FILE);

        return;
    }

    printf(
        "AegisOS: File resized to %d bytes\n",
        MMAP_SIZE
    );

    mapped_memory = mmap(
        NULL,
        MMAP_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        file_descriptor,
        0
    );

    if (mapped_memory == MAP_FAILED)
    {
        perror("AegisOS: mmap");

        close(file_descriptor);
        unlink(MMAP_FILE);

        return;
    }

    printf(
        "AegisOS: File successfully mapped into virtual memory\n"
    );

    memset(
        mapped_memory,
        0,
        MMAP_SIZE
    );

    strncpy(
        mapped_memory,
        initial_message,
        MMAP_SIZE - 1
    );

    mapped_memory[MMAP_SIZE - 1] = '\0';

    printf(
        "AegisOS: Initial mapped data:\n"
    );

    printf(
        "%s\n",
        mapped_memory
    );

    printf(
        "AegisOS: Modifying data directly through mapped memory...\n"
    );

    memset(
        mapped_memory,
        0,
        MMAP_SIZE
    );

    strncpy(
        mapped_memory,
        updated_message,
        MMAP_SIZE - 1
    );

    mapped_memory[MMAP_SIZE - 1] = '\0';

    if (msync(
            mapped_memory,
            MMAP_SIZE,
            MS_SYNC) == -1)
    {
        perror("AegisOS: msync");

        munmap(mapped_memory, MMAP_SIZE);
        close(file_descriptor);
        unlink(MMAP_FILE);

        return;
    }

    printf(
        "AegisOS: Modified data synchronized to file\n"
    );

    printf(
        "AegisOS: Current mapped data:\n"
    );

    printf(
        "%s\n",
        mapped_memory
    );

    printf(
        "AegisOS: Virtual memory address: %p\n",
        (void *)mapped_memory
    );

    if (munmap(
            mapped_memory,
            MMAP_SIZE) == -1)
    {
        perror("AegisOS: munmap");

        close(file_descriptor);
        unlink(MMAP_FILE);

        return;
    }

    printf(
        "AegisOS: Memory mapping successfully released\n"
    );

    close(file_descriptor);

    unlink(MMAP_FILE);

    printf(
        "AegisOS: Temporary mapped file removed\n"
    );

    printf(
        "AegisOS: Virtual memory demonstration completed\n"
    );
}
