#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "diagnostics.h"

static void demonstrate_file_error(void)
{
    int file_descriptor;

    printf(
        "\nAegisOS: Testing file error handling...\n"
    );

    errno = 0;

    file_descriptor = open(
        "aegis_file_that_does_not_exist.txt",
        O_RDONLY
    );

    if (file_descriptor == -1)
    {
        printf(
            "AegisOS: File operation failed as expected\n"
        );

        printf(
            "AegisOS: errno value : %d\n",
            errno
        );

        printf(
            "AegisOS: strerror()  : %s\n",
            strerror(errno)
        );

        perror(
            "AegisOS: perror()"
        );

        return;
    }

    close(file_descriptor);
}

static void demonstrate_directory_error(void)
{
    int result;

    printf(
        "\nAegisOS: Testing directory error handling...\n"
    );

    errno = 0;

    result = rmdir(
        "aegis_directory_that_does_not_exist"
    );

    if (result == -1)
    {
        printf(
            "AegisOS: Directory operation failed as expected\n"
        );

        printf(
            "AegisOS: errno value : %d\n",
            errno
        );

        printf(
            "AegisOS: strerror()  : %s\n",
            strerror(errno)
        );

        perror(
            "AegisOS: perror()"
        );

        return;
    }
}

static void demonstrate_invalid_input(void)
{
    const char *number_text = "not_a_number";
    char *end_pointer;
    long number;

    printf(
        "\nAegisOS: Testing input validation...\n"
    );

    errno = 0;

    end_pointer = NULL;

    number = strtol(
        number_text,
        &end_pointer,
        10
    );

    if (end_pointer == number_text ||
        *end_pointer != '\0')
    {
        printf(
            "AegisOS: Invalid numeric input detected\n"
        );

        printf(
            "AegisOS: Input value : %s\n",
            number_text
        );

        printf(
            "AegisOS: Error       : %s\n",
            "Input is not a valid integer"
        );

        return;
    }

    if (errno == ERANGE)
    {
        printf(
            "AegisOS: Numeric value is outside the valid range\n"
        );

        return;
    }

    printf(
        "AegisOS: Parsed number: %ld\n",
        number
    );
}

void run_diagnostics_demo(void)
{
    printf(
        "AegisOS: Starting error handling and diagnostics demonstration\n"
    );

    printf(
        "AegisOS: Demonstrating errno, perror(), strerror(), and input validation\n"
    );

    demonstrate_file_error();

    demonstrate_directory_error();

    demonstrate_invalid_input();

    printf(
        "\nAegisOS: Diagnostics demonstration completed successfully\n"
    );
}
