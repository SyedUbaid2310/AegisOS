#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>

#include "inspect.h"

#define STATUS_BUFFER_SIZE 16384
#define STAT_BUFFER_SIZE 4096
#define LINE_BUFFER_SIZE 512
#define PATH_BUFFER_SIZE 4096

typedef struct
{
    long pid;
    long parent_pid;

    char state[64];

    unsigned long virtual_memory_kb;
    unsigned long resident_memory_kb;
    unsigned long data_memory_kb;
    unsigned long stack_memory_kb;
    unsigned long thread_count;

    unsigned long long process_cpu_ticks;

    char executable[PATH_MAX];
} ProcessInfo;

static int read_file(
    const char *path,
    char *buffer,
    size_t buffer_size)
{
    FILE *file;
    size_t bytes_read;

    file = fopen(path, "r");

    if (file == NULL)
    {
        return 0;
    }

    bytes_read = fread(
        buffer,
        1,
        buffer_size - 1,
        file
    );

    fclose(file);

    buffer[bytes_read] = '\0';

    return 1;
}

static int parse_status(
    pid_t pid,
    ProcessInfo *info)
{
    char path[PATH_BUFFER_SIZE];
    char buffer[STATUS_BUFFER_SIZE];
    char *line;

    snprintf(
        path,
        sizeof(path),
        "/proc/%ld/status",
        (long)pid
    );

    if (!read_file(
            path,
            buffer,
            sizeof(buffer)))
    {
        return 0;
    }

    line = strtok(buffer, "\n");

    while (line != NULL)
    {
        if (strncmp(line, "PPid:", 5) == 0)
        {
            sscanf(
                line + 5,
                "%ld",
                &info->parent_pid
            );
        }
        else if (strncmp(line, "State:", 6) == 0)
        {
            sscanf(
                line + 6,
                " %63[^\n]",
                info->state
            );
        }
        else if (strncmp(line, "VmSize:", 7) == 0)
        {
            sscanf(
                line + 7,
                "%lu",
                &info->virtual_memory_kb
            );
        }
        else if (strncmp(line, "VmRSS:", 6) == 0)
        {
            sscanf(
                line + 6,
                "%lu",
                &info->resident_memory_kb
            );
        }
        else if (strncmp(line, "VmData:", 7) == 0)
        {
            sscanf(
                line + 7,
                "%lu",
                &info->data_memory_kb
            );
        }
        else if (strncmp(line, "VmStk:", 6) == 0)
        {
            sscanf(
                line + 6,
                "%lu",
                &info->stack_memory_kb
            );
        }
        else if (strncmp(line, "Threads:", 8) == 0)
        {
            sscanf(
                line + 8,
                "%lu",
                &info->thread_count
            );
        }

        line = strtok(NULL, "\n");
    }

    return 1;
}

static int parse_stat(
    pid_t pid,
    ProcessInfo *info)
{
    char path[PATH_BUFFER_SIZE];
    char buffer[STAT_BUFFER_SIZE];

    char *right_parenthesis;
    char *fields;
    char *token;

    int field_number = 3;

    unsigned long long user_ticks = 0;
    unsigned long long system_ticks = 0;

    snprintf(
        path,
        sizeof(path),
        "/proc/%ld/stat",
        (long)pid
    );

    if (!read_file(
            path,
            buffer,
            sizeof(buffer)))
    {
        return 0;
    }

    /*
     * /proc/<pid>/stat contains the process name inside
     * parentheses. The name itself may contain spaces.
     *
     * Therefore locate the final ')' first and begin
     * token parsing after it.
     */

    right_parenthesis = strrchr(
        buffer,
        ')'
    );

    if (right_parenthesis == NULL)
    {
        return 0;
    }

    fields = right_parenthesis + 2;

    token = strtok(
        fields,
        " "
    );

    while (token != NULL)
    {
        if (field_number == 3)
        {
            info->state[0] = token[0];
            info->state[1] = '\0';
        }
        else if (field_number == 4)
        {
            info->parent_pid = strtol(
                token,
                NULL,
                10
            );
        }
        else if (field_number == 14)
        {
            user_ticks = strtoull(
                token,
                NULL,
                10
            );
        }
        else if (field_number == 15)
        {
            system_ticks = strtoull(
                token,
                NULL,
                10
            );

            break;
        }

        field_number++;

        token = strtok(
            NULL,
            " "
        );
    }

    if (field_number < 15)
    {
        return 0;
    }

    info->process_cpu_ticks =
        user_ticks + system_ticks;

    return 1;
}

static void read_executable(
    pid_t pid,
    ProcessInfo *info)
{
    char path[PATH_BUFFER_SIZE];

    snprintf(
        path,
        sizeof(path),
        "/proc/%ld/exe",
        (long)pid
    );

    ssize_t length = readlink(
        path,
        info->executable,
        sizeof(info->executable) - 1
    );

    if (length == -1)
    {
        snprintf(
            info->executable,
            sizeof(info->executable),
            "Unavailable"
        );

        return;
    }

    info->executable[length] = '\0';
}

static int collect_process_info(
    pid_t pid,
    ProcessInfo *info)
{
    memset(
        info,
        0,
        sizeof(*info)
    );

    info->pid = pid;

    if (!parse_status(
            pid,
            info))
    {
        return 0;
    }

    if (!parse_stat(
            pid,
            info))
    {
        return 0;
    }

    read_executable(
        pid,
        info
    );

    return 1;
}

static void print_open_files(pid_t pid)
{
    char fd_directory_path[PATH_BUFFER_SIZE];

    snprintf(
        fd_directory_path,
        sizeof(fd_directory_path),
        "/proc/%ld/fd",
        (long)pid
    );

    DIR *directory = opendir(
        fd_directory_path
    );

    if (directory == NULL)
    {
        printf(
            "Unable to access process file descriptors: %s\n",
            strerror(errno)
        );

        return;
    }

    printf(
        "%-6s %-12s %s\n",
        "FD",
        "TYPE",
        "TARGET"
    );

    printf(
        "------------------------------------------------------------\n"
    );

    struct dirent *entry;

    int displayed_entries = 0;

    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char link_path[
            PATH_BUFFER_SIZE + NAME_MAX + 2
        ];

        char target_path[PATH_BUFFER_SIZE];

        int path_result = snprintf(
            link_path,
            sizeof(link_path),
            "%s/%s",
            fd_directory_path,
            entry->d_name
        );

        if (path_result < 0 ||
            (size_t)path_result >= sizeof(link_path))
        {
            continue;
        }

        ssize_t length = readlink(
            link_path,
            target_path,
            sizeof(target_path) - 1
        );

        if (length == -1)
        {
            continue;
        }

        target_path[length] = '\0';

        const char *type = "other";

        if (strncmp(
                target_path,
                "/dev/",
                5) == 0)
        {
            type = "device";
        }
        else if (strncmp(
                     target_path,
                     "pipe:",
                     5) == 0)
        {
            type = "pipe";
        }
        else if (strncmp(
                     target_path,
                     "socket:",
                     7) == 0)
        {
            type = "socket";
        }
        else if (target_path[0] == '/')
        {
            type = "file";
        }

        printf(
            "%-6s %-12s %s\n",
            entry->d_name,
            type,
            target_path
        );

        displayed_entries++;
    }

    if (displayed_entries == 0)
    {
        printf(
            "No accessible file descriptors found.\n"
        );
    }

    closedir(directory);
}

static void print_cpu_information(void)
{
    FILE *cpu_file;
    char line[LINE_BUFFER_SIZE];

    char model[256];

    long processor_count = 0;

    model[0] = '\0';

    cpu_file = fopen(
        "/proc/cpuinfo",
        "r"
    );

    if (cpu_file == NULL)
    {
        printf(
            "CPU information unavailable\n"
        );

        return;
    }

    while (fgets(
        line,
        sizeof(line),
        cpu_file) != NULL)
    {
        if (strncmp(
                line,
                "processor",
                9) == 0)
        {
            processor_count++;
        }

        if (strncmp(
                line,
                "model name",
                10) == 0 &&
            model[0] == '\0')
        {
            char *separator = strchr(
                line,
                ':'
            );

            if (separator != NULL)
            {
                separator++;

                while (*separator == ' ' ||
                       *separator == '\t')
                {
                    separator++;
                }

                strncpy(
                    model,
                    separator,
                    sizeof(model) - 1
                );

                model[sizeof(model) - 1] = '\0';

                model[strcspn(
                    model,
                    "\n"
                )] = '\0';
            }
        }
    }

    fclose(cpu_file);

    printf(
        "CPU Model          : %s\n",
        model[0] != '\0'
            ? model
            : "Unavailable"
    );

    printf(
        "Logical CPUs        : %ld\n",
        processor_count
    );

    long page_size = sysconf(
        _SC_PAGESIZE
    );

    if (page_size > 0)
    {
        printf(
            "Page Size           : %ld bytes\n",
            page_size
        );
    }
}

static void print_system_memory(void)
{
    FILE *memory_file;
    char line[LINE_BUFFER_SIZE];

    unsigned long total = 0;
    unsigned long available = 0;

    memory_file = fopen(
        "/proc/meminfo",
        "r"
    );

    if (memory_file == NULL)
    {
        return;
    }

    while (fgets(
        line,
        sizeof(line),
        memory_file) != NULL)
    {
        if (sscanf(
                line,
                "MemTotal: %lu kB",
                &total) == 1)
        {
            continue;
        }

        if (sscanf(
                line,
                "MemAvailable: %lu kB",
                &available) == 1)
        {
            continue;
        }
    }

    fclose(memory_file);

    printf(
        "System Memory       : %lu kB total\n",
        total
    );

    printf(
        "Available Memory    : %lu kB\n",
        available
    );
}

static double calculate_cpu_usage(
    unsigned long long first_ticks,
    unsigned long long second_ticks,
    double elapsed_seconds)
{
    long clock_ticks = sysconf(
        _SC_CLK_TCK
    );

    if (clock_ticks <= 0 ||
        elapsed_seconds <= 0)
    {
        return 0.0;
    }

    unsigned long long tick_difference;

    if (second_ticks >= first_ticks)
    {
        tick_difference =
            second_ticks - first_ticks;
    }
    else
    {
        tick_difference = 0;
    }

    return (
        ((double)tick_difference /
         (double)clock_ticks) /
        elapsed_seconds
    ) * 100.0;
}

void run_process_inspector(
    const char *pid_text)
{
    char *end_pointer;

    errno = 0;

    long pid_value = strtol(
        pid_text,
        &end_pointer,
        10
    );

    if (errno != 0 ||
        end_pointer == pid_text ||
        *end_pointer != '\0' ||
        pid_value <= 0)
    {
        printf(
            "AegisOS: Invalid PID: %s\n",
            pid_text
        );

        return;
    }

    pid_t pid = (pid_t)pid_value;

    ProcessInfo first_info;
    ProcessInfo second_info;

    printf("\n");

    printf(
        "============================================================\n"
    );

    printf(
        "              AegisOS PROCESS INSPECTOR\n"
    );

    printf(
        "============================================================\n"
    );

    printf(
        "AegisOS: Inspecting process %ld\n",
        (long)pid
    );

    if (!collect_process_info(
            pid,
            &first_info))
    {
        printf(
            "AegisOS: Unable to inspect process %ld\n",
            (long)pid
        );

        printf(
            "AegisOS: The process may not exist or may have exited.\n"
        );

        return;
    }

    printf(
        "AegisOS: Sampling CPU usage for 1 second...\n"
    );

    sleep(1);

    if (!collect_process_info(
            pid,
            &second_info))
    {
        printf(
            "AegisOS: Process %ld exited during inspection.\n",
            (long)pid
        );

        return;
    }

    long clock_ticks = sysconf(
        _SC_CLK_TCK
    );

    double cpu_usage = calculate_cpu_usage(
        first_info.process_cpu_ticks,
        second_info.process_cpu_ticks,
        1.0
    );

    double cpu_time = 0.0;

    if (clock_ticks > 0)
    {
        cpu_time =
            (double)second_info.process_cpu_ticks /
            (double)clock_ticks;
    }

    printf("\n");

    printf(
        "-------------------- PROCESS ------------------------------\n"
    );

    printf(
        "PID                : %ld\n",
        (long)pid
    );

    printf(
        "Parent PID         : %ld\n",
        second_info.parent_pid
    );

    printf(
        "State              : %s\n",
        second_info.state
    );

    printf(
        "Threads            : %lu\n",
        second_info.thread_count
    );

    printf(
        "Executable         : %s\n",
        second_info.executable
    );

    printf("\n");

    printf(
        "---------------------- CPU --------------------------------\n"
    );

    printf(
        "CPU Usage          : %.2f %%\n",
        cpu_usage
    );

    printf(
        "CPU Time           : %.2f seconds\n",
        cpu_time
    );

    print_cpu_information();

    printf("\n");

    printf(
        "-------------------- MEMORY -------------------------------\n"
    );

    printf(
        "Virtual Memory     : %lu kB\n",
        second_info.virtual_memory_kb
    );

    printf(
        "Resident Memory    : %lu kB\n",
        second_info.resident_memory_kb
    );

    printf(
        "Data Memory        : %lu kB\n",
        second_info.data_memory_kb
    );

    printf(
        "Stack Memory       : %lu kB\n",
        second_info.stack_memory_kb
    );

    print_system_memory();

    printf("\n");

    printf(
        "------------------ OPEN FILES -----------------------------\n"
    );

    print_open_files(pid);

    printf("\n");

    printf(
        "============================================================\n"
    );

    printf(
        "AegisOS: Process inspection completed\n"
    );

    printf(
        "============================================================\n"
    );

    printf("\n");
}
