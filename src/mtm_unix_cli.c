#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mtm.h"

// Forward declarations - internal
static mtimer_t *create_timers(int *count);
static void print_title(char *title);
static int cleanup(const int timer_count, mtimer_t *timers);

/* ========================================================================== */
/* The following functions are required by all implementations                */
/* ========================================================================== */

/**
 * UI-specific setup
 */
int init_ui(void)
{
    initscr();
    raw();
    //noecho();
    //keypad(stdscr, TRUE);

    // TODO: Return any error code as necessary
    return 0;
}

/**
 * UI-specific cleanup
 */
void teardown_ui(void)
{
    endwin();
}

/**
 * The core workhorse code
 * - Create timer(s)
 * - Run timer(s)
 */
int run(void)
{
    int tcount = 0;
    mtimer_t *timers = create_timers(&tcount);

    for (int i = 0; i < tcount; i++) {
        clear();
        if (timers[i].title)
            print_title(timers[i].title);
        run_timer(timers[i]);
    }

    cleanup(tcount, timers);

    return 0;
}

/**
 * Display timer to user
 */
void display_time(long seconds)
{
    int32_t h = seconds / 60 / 60;
    int32_t m = (seconds - h * 60 * 60) / 60;
    int32_t s = seconds - h * 60 * 60 - m * 60;
    printw("\r%02d:%02d:%02d", h, m, s);
    refresh();
}

/**
 * Take a message string and print to stdout
 * Has to accept void * in order to be compatible with multi timer callback
 */
void notify(void *message)
{
    if (!message) return;

    char *msg = (char *)message;
    printw("\n%s", msg);

    printw("\nPress any key to continue...");
    getch();
    clear();
}

/* ========================================================================== */
/* The following functions are specific to this implementation                */
/* ========================================================================== */

/**
 * Create timers from user input
 */
mtimer_t *create_timers(int *tcount)
{
    mtimer_t *timers = NULL;

    // TODO: Loop to build timer list
    while (1) {
        clear();

        char new = 0;
        printw("Create new timer (y/N)? ");
        scanw("%c", &new);
        if (new != 'Y' && new != 'y')
            break;

        long duration = 0;
        char title_input[64] = {};
        char msg_input[64] = {};

        // TODO:
        // - Allow durations to be set in seconds, minutes or hours
        //   e.g. "10s", "3m", "1h"
        do {
            printw("Duration: ");
            scanw("%lu", &duration);
        } while (!duration);

        printw("Title: ");
        getstr(title_input);
        printw("Message: ");
        getstr(msg_input);

        char *title = NULL, *msg = NULL;

        if (title_input[0] != '\0') {
            int len = strlen(title_input) + 1;
            title = malloc(len);
            strncpy(title, title_input, len);
        }
        if (msg_input[0] != '\0') {
            int len = strlen(msg_input) + 1;
            msg = malloc(len);
            strncpy(msg, msg_input, len);
        }

        timers = realloc(timers, ++*tcount * sizeof(mtimer_t));
        timers[*tcount-1] = new_timer(.title=title, .duration=duration,
                .on_display=display_time, .on_complete=notify, .data=msg);
    }

    return timers;
}

/**
 * Display timer title text
 * Expects title to be null-terminated
 */
void print_title(char *title)
{
    printw("%s\n", title);
}

/**
 * Cleanup
 */
int cleanup(const int tcount, mtimer_t *timers)
{
    // TODO: Clean up malloc'd strings
    return 0;
}
