#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

void add_job(pid_t pid);
void show_jobs(void);
void cleanup_jobs(void);

#endif
