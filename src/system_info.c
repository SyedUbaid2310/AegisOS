#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "system_info.h"

#define CPUINFO_SIZE 16384
#define MEMINFO_SIZE 8192

static void read_cpu_information(void)
{
    FILE *cpu_file;
    char line[512];
    char cpu_model[256];
    long processor_count = 0;

    cpu_model[0] = '\0';

    cpu_file = fopen("/proc/cpuinfo", "r");

    if (cpu_file == NULL)
    {
        perror("AegisOS: fopen /proc/cpuinfo");
        return;
    }

    while (fgets(line, sizeof(line), cpu_file) != NULL)
    {
        if (strncmp(line, "processor", 9) == 0)
        {
            processor_count++;
        }

        if (strncmp(line, "model name", 10) == 0 &&
            cpu_model[0] == '\0')
        {
            char *separator = strchr(line, ':');

            if (separator != NULL)
            {
                separator++;

                while (*separator == ' ' ||
                       *separator == '\t')
                {
                    separator++;
                }

                strncpy(
                    cpu_model,
                    separator,
                    sizeof(cpu_model) - 1
                );

                cpu_model[sizeof(cpu_model) - 1] = '\0';

                cpu_model[strcspn(cpu_model, "\n")] = '\0';
            }
        }
    }

    fclose(cpu_file);

    printf(
        "CPU Model              : %s\n",
        cpu_model[0] != '\0'
            ? cpu_model
            : "Unavailable"
    );

    printf(
        "Logical Processors     : %ld\n",
        processor_count
    );
}

static void read_memory_information(void)
{
    FILE *memory_file;
    char line[256];

    unsigned long total_memory_kb = 0;
    unsigned long available_memory_kb = 0;

    memory_file = fopen("/proc/meminfo", "r");

    if (memory_file == NULL)
    {
        perror("AegisOS: fopen /proc/meminfo");
        return;
    }

    while (fgets(line, sizeof(line), memory_file) != NULL)
    {
        if (sscanf(
                line,
                "MemTotal: %lu kB",
                &total_memory_kb) == 1)
        {
            continue;
        }

        if (sscanf(
                line,
                "MemAvailable: %lu kB",
                &available_memory_kb) == 1)
        {
            continue;
        }
    }

    fclose(memory_file);

    printf(
        "Total Memory          : %lu kB\n",
        total_memory_kb
    );

    printf(
        "Available Memory      : %lu kB\n",
        available_memory_kb
    );

    if (total_memory_kb > 0)
    {
        unsigned long used_memory_kb =
            total_memory_kb - available_memory_kb;

        printf(
            "Estimated Used Memory : %lu kB\n",
            used_memory_kb
        );
    }
}

void run_system_info(void)
{
    long page_size;
    long processor_count;

    printf(
        "AegisOS: Starting system information demonstration\n"
    );

    printf(
        "AegisOS: Reading CPU and memory information\n"
    );

    page_size = sysconf(_SC_PAGESIZE);
    processor_count = sysconf(_SC_NPROCESSORS_ONLN);

    printf("\n");
    printf(
        "========== AegisOS System Information ==========\n"
    );

    if (processor_count > 0)
    {
        printf(
            "Online Processors      : %ld\n",
            processor_count
        );
    }
    else
    {
        printf(
            "Online Processors      : Unavailable\n"
        );
    }

    if (page_size > 0)
    {
        printf(
            "Memory Page Size       : %ld bytes\n",
            page_size
        );
    }
    else
    {
        printf(
            "Memory Page Size       : Unavailable\n"
        );
    }

    read_cpu_information();

    read_memory_information();

    printf(
        "=================================================\n"
    );

    printf(
        "AegisOS: System information demonstration completed\n"
    );
}
