#include "shell.h"
#include "signals.h"

int main(void)
{
    setup_signal_handlers();
    shell_run();

    return 0;
}
