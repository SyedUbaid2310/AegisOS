#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "message_queue.h"

#define PARENT_QUEUE_NAME "/aegisos_parent_queue"
#define CHILD_QUEUE_NAME "/aegisos_child_queue"

#define QUEUE_MAX_MESSAGES 10
#define QUEUE_MESSAGE_SIZE 256

static void cleanup_queues(void)
{
    mq_unlink(PARENT_QUEUE_NAME);
    mq_unlink(CHILD_QUEUE_NAME);
}

void run_message_queue_demo(void)
{
    mqd_t parent_queue;
    mqd_t child_queue;
    pid_t child_pid;

    struct mq_attr attributes;

    const char request[] =
        "Hello child, this is the AegisOS parent via message queue!";

    const char response[] =
        "Hello parent, message received through POSIX message queue!";

    char buffer[QUEUE_MESSAGE_SIZE];

    printf(
        "AegisOS: Starting POSIX message queue demonstration\n"
    );

    cleanup_queues();

    attributes.mq_flags = 0;
    attributes.mq_maxmsg = QUEUE_MAX_MESSAGES;
    attributes.mq_msgsize = QUEUE_MESSAGE_SIZE;
    attributes.mq_curmsgs = 0;

    parent_queue = mq_open(
        PARENT_QUEUE_NAME,
        O_CREAT | O_RDWR,
        0666,
        &attributes
    );

    if (parent_queue == (mqd_t)-1)
    {
        perror("AegisOS: mq_open parent queue");
        return;
    }

    child_queue = mq_open(
        CHILD_QUEUE_NAME,
        O_CREAT | O_RDWR,
        0666,
        &attributes
    );

    if (child_queue == (mqd_t)-1)
    {
        perror("AegisOS: mq_open child queue");

        mq_close(parent_queue);
        mq_unlink(PARENT_QUEUE_NAME);

        return;
    }

    printf(
        "AegisOS: Two message queues created successfully\n"
    );

    child_pid = fork();

    if (child_pid == -1)
    {
        perror("AegisOS: fork");

        mq_close(parent_queue);
        mq_close(child_queue);

        cleanup_queues();

        return;
    }

    if (child_pid == 0)
    {
        ssize_t bytes_received;

        printf(
            "AegisOS: Child waiting for parent message...\n"
        );

        bytes_received = mq_receive(
            parent_queue,
            buffer,
            QUEUE_MESSAGE_SIZE,
            NULL
        );

        if (bytes_received == -1)
        {
            perror("AegisOS: child mq_receive");

            mq_close(parent_queue);
            mq_close(child_queue);

            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';

        printf(
            "AegisOS: Child received: %s\n",
            buffer
        );

        printf(
            "AegisOS: Child sending response...\n"
        );

        if (mq_send(
                child_queue,
                response,
                strlen(response) + 1,
                0) == -1)
        {
            perror("AegisOS: child mq_send");

            mq_close(parent_queue);
            mq_close(child_queue);

            exit(EXIT_FAILURE);
        }

        mq_close(parent_queue);
        mq_close(child_queue);

        printf(
            "AegisOS: Child process finished\n"
        );

        exit(EXIT_SUCCESS);
    }

    printf(
        "AegisOS: Parent sending message...\n"
    );

    if (mq_send(
            parent_queue,
            request,
            strlen(request) + 1,
            0) == -1)
    {
        perror("AegisOS: parent mq_send");

        mq_close(parent_queue);
        mq_close(child_queue);

        waitpid(child_pid, NULL, 0);

        cleanup_queues();

        return;
    }

    printf(
        "AegisOS: Parent waiting for child response...\n"
    );

    ssize_t bytes_received = mq_receive(
        child_queue,
        buffer,
        QUEUE_MESSAGE_SIZE,
        NULL
    );

    if (bytes_received == -1)
    {
        perror("AegisOS: parent mq_receive");

        mq_close(parent_queue);
        mq_close(child_queue);

        waitpid(child_pid, NULL, 0);

        cleanup_queues();

        return;
    }

    buffer[bytes_received] = '\0';

    printf(
        "AegisOS: Parent received: %s\n",
        buffer
    );

    mq_close(parent_queue);
    mq_close(child_queue);

    waitpid(child_pid, NULL, 0);

    printf(
        "AegisOS: Parent detected child completion\n"
    );

    cleanup_queues();

    printf(
        "AegisOS: POSIX message queue demonstration completed\n"
    );
}
