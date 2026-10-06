#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "process_monitor.h"

#define STATUS_FILE_SIZE 8192

static int read_status_value(
    const char *status,
    const char *field,
    char *value,
    size_t value_size)
{
    const char *position;
    const char *line_end;
    size_t field_length;

    field_length = strlen(field);
    position = status;

    while (*position != '\0')
    {
        if (strncmp(position, field, field_length) == 0)
        {
            line_end = strchr(position, '\n');

            if (line_end == NULL)
            {
                line_end = position + strlen(position);
            }

            position += field_length;

            while (*position == ' ' || *position == '\t')
            {
                position++;
            }

            size_t length =
                (size_t)(line_end - position);

            if (length >= value_size)
            {
                length = value_size - 1;
            }

            memcpy(value, position, length);
            value[length] = '\0';

            return 1;
        }

        position = strchr(position, '\n');

        if (position == NULL)
        {
            break;
        }

        position++;
    }

    return 0;
}

void run_process_monitor(void)
{
    char status_path[64];
    char status_data[STATUS_FILE_SIZE];

    char process_state[64];
    char parent_pid[64];
    char memory_usage[64];
    char thread_count[64];

    FILE *status_file;

    pid_t process_id = getpid();

    printf(
        "AegisOS: Starting process monitor\n"
    );

    printf(
        "AegisOS: Reading Linux /proc process information\n"
    );

    snprintf(
        status_path,
        sizeof(status_path),
        "/proc/%ld/status",
        (long)process_id
    );

    status_file = fopen(status_path, "r");

    if (status_file == NULL)
    {
        perror("AegisOS: fopen /proc status");
        return;
    }

    size_t bytes_read = fread(
        status_data,
        1,
        sizeof(status_data) - 1,
        status_file
    );

    fclose(status_file);

    if (bytes_read == 0)
    {
        fprintf(
            stderr,
            "AegisOS: Unable to read process information\n"
        );

        return;
    }

    status_data[bytes_read] = '\0';

    process_state[0] = '\0';
    parent_pid[0] = '\0';
    memory_usage[0] = '\0';
    thread_count[0] = '\0';

    read_status_value(
        status_data,
        "State:",
        process_state,
        sizeof(process_state)
    );

    read_status_value(
        status_data,
        "PPid:",
        parent_pid,
        sizeof(parent_pid)
    );

    read_status_value(
        status_data,
        "VmRSS:",
        memory_usage,
        sizeof(memory_usage)
    );

    read_status_value(
        status_data,
        "Threads:",
        thread_count,
        sizeof(thread_count)
    );

    printf("\n");
    printf(
        "========== AegisOS Process Monitor ==========\n"
    );

    printf(
        "Process ID (PID)       : %ld\n",
        (long)process_id
    );

    printf(
        "Parent Process ID      : %s\n",
        parent_pid[0] != '\0'
            ? parent_pid
            : "Unavailable"
    );

    printf(
        "Process State          : %s\n",
        process_state[0] != '\0'
            ? process_state
            : "Unavailable"
    );

    printf(
        "Resident Memory        : %s\n",
        memory_usage[0] != '\0'
            ? memory_usage
            : "Unavailable"
    );

    printf(
        "Thread Count           : %s\n",
        thread_count[0] != '\0'
            ? thread_count
            : "Unavailable"
    );

    printf(
        "Proc Status Source     : %s\n",
        status_path
    );

    printf(
        "=============================================\n"
    );

    printf(
        "AegisOS: Process monitoring completed\n"
    );
}
