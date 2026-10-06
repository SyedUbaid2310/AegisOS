#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

#define INITIAL_SIZE 3
#define EXPANDED_SIZE 5

void run_memory_demo(void)
{
    int *memory_block;

    printf(
        "AegisOS: Starting memory management demonstration\n"
    );

    printf(
        "AegisOS: Allocating memory for %d integers using malloc...\n",
        INITIAL_SIZE
    );

    memory_block = malloc(
        INITIAL_SIZE * sizeof(int)
    );

    if (memory_block == NULL)
    {
        perror("AegisOS: malloc");
        return;
    }

    for (int i = 0; i < INITIAL_SIZE; i++)
    {
        memory_block[i] = (i + 1) * 10;
    }

    printf(
        "AegisOS: malloc allocation successful\n"
    );

    printf(
        "AegisOS: Initial memory values: "
    );

    for (int i = 0; i < INITIAL_SIZE; i++)
    {
        printf("%d ", memory_block[i]);
    }

    printf("\n");

    printf(
        "AegisOS: Expanding memory to %d integers using realloc...\n",
        EXPANDED_SIZE
    );

    int *expanded_block = realloc(
        memory_block,
        EXPANDED_SIZE * sizeof(int)
    );

    if (expanded_block == NULL)
    {
        perror("AegisOS: realloc");

        free(memory_block);

        return;
    }

    memory_block = expanded_block;

    memory_block[3] = 40;
    memory_block[4] = 50;

    printf(
        "AegisOS: realloc expansion successful\n"
    );

    printf(
        "AegisOS: Expanded memory values: "
    );

    for (int i = 0; i < EXPANDED_SIZE; i++)
    {
        printf("%d ", memory_block[i]);
    }

    printf("\n");

    printf(
        "AegisOS: Allocating zero-initialized memory using calloc...\n"
    );

    int *zero_block = calloc(
        INITIAL_SIZE,
        sizeof(int)
    );

    if (zero_block == NULL)
    {
        perror("AegisOS: calloc");

        free(memory_block);

        return;
    }

    printf(
        "AegisOS: calloc allocation successful\n"
    );

    printf(
        "AegisOS: calloc values: "
    );

    for (int i = 0; i < INITIAL_SIZE; i++)
    {
        printf("%d ", zero_block[i]);
    }

    printf("\n");

    printf(
        "AegisOS: Memory allocation addresses:\n"
    );

    printf(
        "AegisOS: malloc/realloc block = %p\n",
        (void *)memory_block
    );

    printf(
        "AegisOS: calloc block            = %p\n",
        (void *)zero_block
    );

    printf(
        "AegisOS: Releasing allocated memory using free...\n"
    );

    free(memory_block);
    free(zero_block);

    printf(
        "AegisOS: Memory successfully released\n"
    );

    printf(
        "AegisOS: Memory management demonstration completed\n"
    );
}
