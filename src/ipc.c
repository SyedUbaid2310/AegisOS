#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "ipc.h"

#define BUFFER_SIZE 256

void run_ipc_demo(void)
{
    int parent_to_child[2];
    int child_to_parent[2];

    pid_t child_pid;

    const char request[] =
        "Hello child, this is the AegisOS parent!";

    const char response[] =
        "Hello parent, message received successfully!";

    char buffer[BUFFER_SIZE];

    printf("AegisOS: Starting bidirectional IPC demonstration\n");

    /*
     * First pipe:
     * Parent -> Child
     */
    if (pipe(parent_to_child) == -1)
    {
        perror("AegisOS: parent_to_child pipe");
        return;
    }

    /*
     * Second pipe:
     * Child -> Parent
     */
    if (pipe(child_to_parent) == -1)
    {
        perror("AegisOS: child_to_parent pipe");

        close(parent_to_child[0]);
        close(parent_to_child[1]);

        return;
    }

    printf("AegisOS: Two IPC pipes created successfully\n");

    child_pid = fork();

    if (child_pid == -1)
    {
        perror("AegisOS: fork");

        close(parent_to_child[0]);
        close(parent_to_child[1]);

        close(child_to_parent[0]);
        close(child_to_parent[1]);

        return;
    }

    /*
     * =========================
     * CHILD PROCESS
     * =========================
     */
    if (child_pid == 0)
    {
        ssize_t bytes_read;

        /*
         * Child only reads from the first pipe
         * and writes to the second pipe.
         */
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        printf("AegisOS: Child waiting for parent message...\n");

        bytes_read = read(
            parent_to_child[0],
            buffer,
            sizeof(buffer) - 1
        );

        if (bytes_read == -1)
        {
            perror("AegisOS: child read");
            close(parent_to_child[0]);
            close(child_to_parent[1]);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_read] = '\0';

        printf(
            "AegisOS: Child received: %s\n",
            buffer
        );

        printf("AegisOS: Child sending response...\n");

        if (write(
                child_to_parent[1],
                response,
                strlen(response)) == -1)
        {
            perror("AegisOS: child write");

            close(parent_to_child[0]);
            close(child_to_parent[1]);

            exit(EXIT_FAILURE);
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);

        printf("AegisOS: Child process finished\n");

        exit(EXIT_SUCCESS);
    }

    /*
     * =========================
     * PARENT PROCESS
     * =========================
     */

    /*
     * Parent writes to the first pipe
     * and reads from the second pipe.
     */
    close(parent_to_child[0]);
    close(child_to_parent[1]);

    printf("AegisOS: Parent sending message...\n");

    if (write(
            parent_to_child[1],
            request,
            strlen(request)) == -1)
    {
        perror("AegisOS: parent write");

        close(parent_to_child[1]);
        close(child_to_parent[0]);

        waitpid(child_pid, NULL, 0);

        return;
    }

    close(parent_to_child[1]);

    printf("AegisOS: Parent waiting for child response...\n");

    ssize_t bytes_read = read(
        child_to_parent[0],
        buffer,
        sizeof(buffer) - 1
    );

    if (bytes_read == -1)
    {
        perror("AegisOS: parent read");

        close(child_to_parent[0]);

        waitpid(child_pid, NULL, 0);

        return;
    }

    buffer[bytes_read] = '\0';

    printf(
        "AegisOS: Parent received: %s\n",
        buffer
    );

    close(child_to_parent[0]);

    waitpid(child_pid, NULL, 0);

    printf("AegisOS: Parent detected child completion\n");
    printf("AegisOS: Bidirectional IPC demonstration completed\n");
}
