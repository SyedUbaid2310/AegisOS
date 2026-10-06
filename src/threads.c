#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "threads.h"

#define THREAD_COUNT 3
#define THREAD_STEPS 5

static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

static void sleep_for_100ms(void)
{
    struct timespec delay;

    delay.tv_sec = 0;
    delay.tv_nsec = 100000000L;

    nanosleep(&delay, NULL);
}

static void *thread_function(void *arg)
{
    int thread_id = *(int *)arg;

    for (int step = 1; step <= THREAD_STEPS; step++)
    {
        pthread_mutex_lock(&print_mutex);

        printf(
            "AegisOS: Thread %d is running - step %d/%d\n",
            thread_id,
            step,
            THREAD_STEPS
        );

        pthread_mutex_unlock(&print_mutex);

        /*
         * Give the scheduler an opportunity to run
         * another thread.
         */
        sleep_for_100ms();
    }

    pthread_mutex_lock(&print_mutex);

    printf(
        "AegisOS: Thread %d finished\n",
        thread_id
    );

    pthread_mutex_unlock(&print_mutex);

    return NULL;
}

void run_thread_demo(void)
{
    pthread_t threads[THREAD_COUNT];
    int thread_ids[THREAD_COUNT];

    printf("AegisOS: Starting thread scheduling demonstration\n");
    printf("AegisOS: Creating %d threads...\n", THREAD_COUNT);

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        thread_ids[i] = i + 1;

        if (pthread_create(
                &threads[i],
                NULL,
                thread_function,
                &thread_ids[i]) != 0)
        {
            perror("AegisOS: pthread_create");
            return;
        }
    }

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("AegisOS: pthread_join");
            return;
        }
    }

    printf("AegisOS: All threads completed\n");
}
