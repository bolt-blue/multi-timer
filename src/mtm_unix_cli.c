#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mtm.h"

#define MAX_STR 64

// Forward declarations - internal
static int create_timers(void);
static void print_title(char *title);
static int cleanup(void);

static WINDOW *win, *boxwin;

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

    int w, h;
    getmaxyx(stdscr, h, w);

    int boxw = 44, boxh = 12;
    boxwin = newwin(boxh, boxw, h/2 - boxh/2, w/2 - boxw/2);
    win = derwin(boxwin, boxh-2, boxw-4, 1, 2);
    refresh();

    box(boxwin, 0, 0);
    wrefresh(boxwin);

    // TODO: Return any error code as necessary
    return 0;
}

/**
 * UI-specific cleanup
 */
void teardown_ui(void)
{
    delwin(win);
    endwin();
}

/**
 * The core workhorse code
 * - Create timer(s)
 * - Run timer(s)
 */
int run(void)
{
    // TODO: Error handling
    create_timers();
    int tcount = num_timers();

    for (int i = 0; i < tcount; i++) {
        wclear(win);
        mtimer_t timer = get_timer(i);
        if (timer.title)
            print_title(timer.title);
        // Allow timer to be killed by user
        // TODO: Catch Ctl-C instead so we don't kill the whole program if
        // there are multiple timers
        cbreak();
        run_timer(timer);
        nocbreak();
    }

    cleanup();
    free_timers();

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
    wprintw(win, "\r%02d:%02d:%02d", h, m, s);
    wrefresh(win);
}

/**
 * Take a message string and print to stdout
 * Has to accept void * in order to be compatible with multi timer callback
 */
void notify(void *message)
{
    if (!message) return;

    char *msg = (char *)message;
    mvwprintw(win, 3, 0, "%s", msg);
    mvwprintw(win, 4, 0, "Press any key to continue...");

    wgetch(win);
    wclear(win);
}

/* ========================================================================== */
/* The following functions are specific to this implementation                */
/* ========================================================================== */

/**
 * Create timers from user input
 */
int create_timers(void)
{
    // TODO: Loop to build timer list
    while (1) {
        wclear(win);

        char new = 0;
        wprintw(win, "Create new timer (y/N)? ");
        wscanw(win, "%c", &new);
        if (new != 'Y' && new != 'y')
            break;

        long duration = 0;
        char title_input[MAX_STR] = {};
        char msg_input[MAX_STR] = {};

        // TODO:
        // - Allow durations to be set in seconds, minutes or hours
        //   e.g. "10s", "3m", "1h"
        do {
            mvwprintw(win, 1, 0, "Duration: ");
            wscanw(win, "%lu", &duration);
        } while (!duration);

        mvwprintw(win, 2, 0, "Title: ");
        wgetnstr(win, title_input, MAX_STR);
        mvwprintw(win, 3, 0,  "Message: ");
        wgetnstr(win, msg_input, MAX_STR);

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

        add_timer(new_timer(.title=title, .duration=duration,
                    .on_display=display_time, .on_complete=notify, .data=msg));
    }

    // TODO: Error handling
    return 0;
}

/**
 * Display timer title text
 * Expects title to be null-terminated
 */
void print_title(char *title)
{
    wprintw(win, "%s\n", title);
}

/**
 * Cleanup
 */
int cleanup(void)
{
    // NOTE: In the current use-case, timer .data is a malloc'd message string.
    // See create_timers()
    int tcount = num_timers();
    for (int i = 0; i < tcount; i++) {
        mtimer_t t = get_timer(i);
        free(t.title);
        free(t.data);
    }
    return 0;
}
