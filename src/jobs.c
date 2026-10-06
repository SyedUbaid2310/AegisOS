#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "jobs.h"

#define MAX_JOBS 100

static pid_t jobs[MAX_JOBS];
static int job_count = 0;

void add_job(pid_t pid)
{
    if (job_count >= MAX_JOBS)
    {
        fprintf(stderr, "AegisOS: maximum number of jobs reached\n");
        return;
    }

    jobs[job_count] = pid;
    job_count++;

    printf("[job %d] started with PID %d\n",
           job_count,
           (int)pid);
}

void cleanup_jobs(void)
{
    int i = 0;

    while (i < job_count)
    {
        pid_t result = waitpid(jobs[i], NULL, WNOHANG);

        if (result == jobs[i])
        {
            printf("[job %d] finished\n", i + 1);

            for (int j = i; j < job_count - 1; j++)
            {
                jobs[j] = jobs[j + 1];
            }

            job_count--;
        }
        else
        {
            i++;
        }
    }
}

void show_jobs(void)
{
    cleanup_jobs();

    if (job_count == 0)
    {
        printf("No background jobs\n");
        return;
    }

    for (int i = 0; i < job_count; i++)
    {
        printf("[%d] PID %d running\n",
               i + 1,
               (int)jobs[i]);
    }
}
