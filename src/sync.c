#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "sync.h"

#define THREAD_COUNT 3
#define THREAD_STEPS 3

static sem_t resource_semaphore;

static void sleep_for_200ms(void)
{
    struct timespec delay;

    delay.tv_sec = 0;
    delay.tv_nsec = 200000000L;

    nanosleep(&delay, NULL);
}

static void *worker_thread(void *arg)
{
    int thread_id = *(int *)arg;

    for (int step = 1; step <= THREAD_STEPS; step++)
    {
        printf(
            "AegisOS: Thread %d waiting for shared resource...\n",
            thread_id
        );

        /*
         * Wait for permission to enter
         * the critical section.
         */
        if (sem_wait(&resource_semaphore) != 0)
        {
            perror("AegisOS: sem_wait");
            return NULL;
        }

        printf(
            "AegisOS: Thread %d entered critical section - step %d/%d\n",
            thread_id,
            step,
            THREAD_STEPS
        );

        /*
         * Simulate work while holding
         * the shared resource.
         */
        sleep_for_200ms();

        printf(
            "AegisOS: Thread %d leaving critical section\n",
            thread_id
        );

        /*
         * Release the shared resource.
         */
        if (sem_post(&resource_semaphore) != 0)
        {
            perror("AegisOS: sem_post");
            return NULL;
        }

        /*
         * Give other threads an opportunity
         * to request the resource.
         */
        sleep_for_200ms();
    }

    printf(
        "AegisOS: Thread %d completed all work\n",
        thread_id
    );

    return NULL;
}

void run_sync_demo(void)
{
    pthread_t threads[THREAD_COUNT];
    int thread_ids[THREAD_COUNT];

    printf("AegisOS: Starting semaphore synchronization demonstration\n");

    /*
     * Initialize the semaphore with value 1.
     *
     * Value 1 means only one thread can
     * enter the critical section at a time.
     */
    if (sem_init(&resource_semaphore, 0, 1) != 0)
    {
        perror("AegisOS: sem_init");
        return;
    }

    printf(
        "AegisOS: Semaphore initialized with value 1\n"
    );

    printf(
        "AegisOS: Creating %d worker threads...\n",
        THREAD_COUNT
    );

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        thread_ids[i] = i + 1;

        if (pthread_create(
                &threads[i],
                NULL,
                worker_thread,
                &thread_ids[i]) != 0)
        {
            perror("AegisOS: pthread_create");

            /*
             * Wait for threads that were already created.
             */
            for (int j = 0; j < i; j++)
            {
                pthread_join(threads[j], NULL);
            }

            sem_destroy(&resource_semaphore);
            return;
        }
    }

    /*
     * Wait for every worker thread.
     */
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("AegisOS: pthread_join");
            sem_destroy(&resource_semaphore);
            return;
        }
    }

    /*
     * Destroy the semaphore after all
     * threads have finished.
     */
    if (sem_destroy(&resource_semaphore) != 0)
    {
        perror("AegisOS: sem_destroy");
        return;
    }

    printf(
        "AegisOS: Semaphore synchronization demonstration completed\n"
    );
}
