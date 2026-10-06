#include <signal.h>
#include <stdio.h>

#include "signals.h"

void handle_sigint(int signal_number)
{
    (void)signal_number;
    printf("\nAegisOS: use Ctrl+D to exit the shell\n");
    fflush(stdout);
}

void handle_sigchld(int signal_number)
{
    (void)signal_number;
}

void setup_signal_handlers(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGCHLD, handle_sigchld);
}
