#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "condition.h"

#define BUFFER_SIZE 3
#define ITEMS_TO_PRODUCE 6

static int buffer[BUFFER_SIZE];

static int buffer_count = 0;
static int buffer_in = 0;
static int buffer_out = 0;

static pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t buffer_not_full =
    PTHREAD_COND_INITIALIZER;

static pthread_cond_t buffer_not_empty =
    PTHREAD_COND_INITIALIZER;

static void sleep_for_200ms(void)
{
    struct timespec delay;

    delay.tv_sec = 0;
    delay.tv_nsec = 200000000L;

    nanosleep(&delay, NULL);
}

static void *producer_thread(void *arg)
{
    (void)arg;

    for (int item = 1; item <= ITEMS_TO_PRODUCE; item++)
    {
        pthread_mutex_lock(&buffer_mutex);

        /*
         * Wait while the buffer is full.
         */
        while (buffer_count == BUFFER_SIZE)
        {
            printf(
                "AegisOS: Producer waiting - buffer is full\n"
            );

            pthread_cond_wait(
                &buffer_not_full,
                &buffer_mutex
            );
        }

        /*
         * Add an item to the shared buffer.
         */
        buffer[buffer_in] = item;

        buffer_in =
            (buffer_in + 1) % BUFFER_SIZE;

        buffer_count++;

        printf(
            "AegisOS: Producer produced item %d "
            "(buffer count = %d)\n",
            item,
            buffer_count
        );

        /*
         * Tell a waiting consumer that
         * data is available.
         */
        pthread_cond_signal(&buffer_not_empty);

        pthread_mutex_unlock(&buffer_mutex);

        sleep_for_200ms();
    }

    printf(
        "AegisOS: Producer completed all work\n"
    );

    return NULL;
}

static void *consumer_thread(void *arg)
{
    (void)arg;

    for (int item = 1; item <= ITEMS_TO_PRODUCE; item++)
    {
        pthread_mutex_lock(&buffer_mutex);

        /*
         * Wait while the buffer is empty.
         */
        while (buffer_count == 0)
        {
            printf(
                "AegisOS: Consumer waiting - buffer is empty\n"
            );

            pthread_cond_wait(
                &buffer_not_empty,
                &buffer_mutex
            );
        }

        /*
         * Remove an item from the shared buffer.
         */
        int value = buffer[buffer_out];

        buffer_out =
            (buffer_out + 1) % BUFFER_SIZE;

        buffer_count--;

        printf(
            "AegisOS: Consumer consumed item %d "
            "(buffer count = %d)\n",
            value,
            buffer_count
        );

        /*
         * Tell a waiting producer that
         * space is available.
         */
        pthread_cond_signal(&buffer_not_full);

        pthread_mutex_unlock(&buffer_mutex);

        sleep_for_200ms();
    }

    printf(
        "AegisOS: Consumer completed all work\n"
    );

    return NULL;
}

void run_condition_demo(void)
{
    pthread_t producer;
    pthread_t consumer;

    /*
     * Reset shared-buffer state.
     */
    buffer_count = 0;
    buffer_in = 0;
    buffer_out = 0;

    printf(
        "AegisOS: Starting condition-variable "
        "producer/consumer demonstration\n"
    );

    printf(
        "AegisOS: Shared buffer size = %d\n",
        BUFFER_SIZE
    );

    printf(
        "AegisOS: Items to produce = %d\n",
        ITEMS_TO_PRODUCE
    );

    /*
     * Create producer thread.
     */
    if (pthread_create(
            &producer,
            NULL,
            producer_thread,
            NULL) != 0)
    {
        perror("AegisOS: pthread_create producer");
        return;
    }

    /*
     * Create consumer thread.
     */
    if (pthread_create(
            &consumer,
            NULL,
            consumer_thread,
            NULL) != 0)
    {
        perror("AegisOS: pthread_create consumer");

        pthread_join(producer, NULL);

        return;
    }

    /*
     * Wait for producer.
     */
    if (pthread_join(producer, NULL) != 0)
    {
        perror("AegisOS: pthread_join producer");
        return;
    }

    /*
     * Wait for consumer.
     */
    if (pthread_join(consumer, NULL) != 0)
    {
        perror("AegisOS: pthread_join consumer");
        return;
    }

    printf(
        "AegisOS: Condition-variable demonstration completed\n"
    );
}
