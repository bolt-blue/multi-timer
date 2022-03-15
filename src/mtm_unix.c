#include <stdio.h>
#include <stdlib.h>

#include "multi_timer.h"

// Forward declarations
void display_time(long s);
void notify(void *);
void usage(char *pname);

// The following handle any necessary forward declared definitions
#ifndef MTM_GUI
#include "mtm_unix_cli.c"
#else
#include "mtm_unix_gui.c"
#endif

int main(int argc, char **argv)
{
    if (argc == 1) {
        usage(argv[0]);
        exit(1);
    }

#if 1
    // TODO: Validation
    long duration = atoi(argv[1]);

    // TODO:
    // - Allow multiple timers to be set
    // - Allow durations to be set in seconds, minutes or hours
    //   e.g. "10s", "3m", "1h"
#else
    for (int i = 1; i < argc; i++) {

    }
#endif

    mtimer_t first_timer = new_timer(.duration=duration, .display=display_time,
            .on_complete=notify, .data="Ding! We're done.");

    run_timer(first_timer);

    return 0;
}

void usage(char *pname)
{
    printf("Usage: %s duration [...]\n", pname);
}
