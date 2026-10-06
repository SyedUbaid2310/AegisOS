#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "filesystem.h"

#define DEMO_DIRECTORY "aegis_fs_demo"
#define ORIGINAL_FILE "aegis_fs_demo/original.txt"
#define RENAMED_FILE "aegis_fs_demo/renamed.txt"

#define FILE_CONTENT \
    "AegisOS filesystem demonstration data.\n" \
    "This file was created, written, read, and renamed by AegisOS.\n"

void run_filesystem_demo(void)
{
    int file_descriptor;
    struct stat file_information;
    char buffer[512];

    printf(
        "AegisOS: Starting file-system operations demonstration\n"
    );

    printf(
        "AegisOS: Creating demonstration directory: %s\n",
        DEMO_DIRECTORY
    );

    if (mkdir(DEMO_DIRECTORY, 0755) == -1)
    {
        if (errno != EEXIST)
        {
            perror("AegisOS: mkdir");
            return;
        }

        printf(
            "AegisOS: Demonstration directory already exists\n"
        );
    }
    else
    {
        printf(
            "AegisOS: Directory created successfully\n"
        );
    }

    printf(
        "AegisOS: Creating file: %s\n",
        ORIGINAL_FILE
    );

    file_descriptor = open(
        ORIGINAL_FILE,
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (file_descriptor == -1)
    {
        perror("AegisOS: open");
        rmdir(DEMO_DIRECTORY);
        return;
    }

    printf(
        "AegisOS: File created successfully\n"
    );

    printf(
        "AegisOS: Writing data to file...\n"
    );

    ssize_t bytes_written = write(
        file_descriptor,
        FILE_CONTENT,
        strlen(FILE_CONTENT)
    );

    if (bytes_written == -1)
    {
        perror("AegisOS: write");

        close(file_descriptor);
        unlink(ORIGINAL_FILE);
        rmdir(DEMO_DIRECTORY);

        return;
    }

    printf(
        "AegisOS: Wrote %ld bytes\n",
        (long)bytes_written
    );

    if (close(file_descriptor) == -1)
    {
        perror("AegisOS: close");
        return;
    }

    printf(
        "AegisOS: File closed successfully\n"
    );

    printf(
        "AegisOS: Reopening file for reading...\n"
    );

    file_descriptor = open(
        ORIGINAL_FILE,
        O_RDONLY
    );

    if (file_descriptor == -1)
    {
        perror("AegisOS: open for reading");

        unlink(ORIGINAL_FILE);
        rmdir(DEMO_DIRECTORY);

        return;
    }

    ssize_t bytes_read = read(
        file_descriptor,
        buffer,
        sizeof(buffer) - 1
    );

    if (bytes_read == -1)
    {
        perror("AegisOS: read");

        close(file_descriptor);
        unlink(ORIGINAL_FILE);
        rmdir(DEMO_DIRECTORY);

        return;
    }

    buffer[bytes_read] = '\0';

    printf(
        "AegisOS: Read %ld bytes from file\n",
        (long)bytes_read
    );

    printf(
        "AegisOS: File contents:\n"
        "%s",
        buffer
    );

    close(file_descriptor);

    printf(
        "AegisOS: Reading file metadata using stat()...\n"
    );

    if (stat(ORIGINAL_FILE, &file_information) == -1)
    {
        perror("AegisOS: stat");

        unlink(ORIGINAL_FILE);
        rmdir(DEMO_DIRECTORY);

        return;
    }

    printf(
        "AegisOS: File size        : %ld bytes\n",
        (long)file_information.st_size
    );

    printf(
        "AegisOS: File permissions  : %o\n",
        file_information.st_mode & 0777
    );

    printf(
        "AegisOS: File inode        : %ld\n",
        (long)file_information.st_ino
    );

    printf(
        "AegisOS: Renaming file to: %s\n",
        RENAMED_FILE
    );

    if (rename(ORIGINAL_FILE, RENAMED_FILE) == -1)
    {
        perror("AegisOS: rename");

        unlink(ORIGINAL_FILE);
        rmdir(DEMO_DIRECTORY);

        return;
    }

    printf(
        "AegisOS: File renamed successfully\n"
    );

    printf(
        "AegisOS: Removing renamed file...\n"
    );

    if (unlink(RENAMED_FILE) == -1)
    {
        perror("AegisOS: unlink");

        rmdir(DEMO_DIRECTORY);

        return;
    }

    printf(
        "AegisOS: File removed successfully\n"
    );

    printf(
        "AegisOS: Removing demonstration directory...\n"
    );

    if (rmdir(DEMO_DIRECTORY) == -1)
    {
        perror("AegisOS: rmdir");
        return;
    }

    printf(
        "AegisOS: Directory removed successfully\n"
    );

    printf(
        "AegisOS: File-system operations demonstration completed\n"
    );
}
