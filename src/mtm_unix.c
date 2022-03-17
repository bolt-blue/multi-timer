#include <stdio.h>
#include <stdlib.h>

#include "multi_timer.h"

// Forward declarations
void print_title(char *title);
void display_time(long seconds);
void notify(void *message);
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

    // TODO:
    // - Allow durations to be set in seconds, minutes or hours
    //   e.g. "10s", "3m", "1h"

    // Set up
#ifndef MTM_GUI
    // Set up ncurses
    initscr();
    raw();
    noecho();
    //keypad(stdscr, TRUE);
#endif

    int tcount = argc - 1;
    mtimer_t timers[tcount];

    for (int i = 0; i < tcount; i++) {
        // TODO: Validation
        long duration = atoi(argv[i+1]);
        timers[i] = new_timer(.title="Some title", .on_display=display_time,
                .duration=duration, .on_complete=notify, .data="Ding! We're done.");
    }

    for (int i = 0; i < tcount; i++) {
        if (timers[i].title) print_title(timers[i].title);

        run_timer(timers[i]);
    }

    // Clean up
#ifndef MTM_GUI
    // Clean up ncurses
    endwin();
#endif

    return 0;
}

void usage(char *pname)
{
    printf("Usage: %s duration [...]\n", pname);
}
