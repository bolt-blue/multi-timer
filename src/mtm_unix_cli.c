#include <form.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mtm.h"

#define MAX_STR 64

// Helper functions
// TODO: Separate out
char *strtriml(char *s);
char *strtrimr(char *s);
char *strtrim(char *s);

// Forward declarations - internal
static int create_timers(void);
static void wprint_window_title(WINDOW *win, char *title);
static void print_title(char *title);
static void wprint_status(WINDOW *win, char*status);
static void wclear_status(WINDOW *win);
static int cleanup(void);

#define print_status(...) wprint_status(stdscr, __VA_ARGS__)
#define clear_status() wclear_status(stdscr)

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
    noecho();
    cbreak(); /* allow C-c to behave normally */
    keypad(stdscr, TRUE);

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

    print_status("Press any key to start your timer(s)");
    wgetch(win);
    clear_status();

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
void notify(void *data)
{
    if (!data) return;
    Notification n = *(Notification *)data;

    mvwprintw(win, 2, 0, "%s", n.msg);
    mvwprintw(win, 4, 0, "TODO: Play '%s'", n.aud);

    flash();

    print_status("Press any key to continue");
    wgetch(win);
    wclear(win);
    clear_status();
}

/* ========================================================================== */
/* The following functions are specific to this implementation (unix)         */
/* ========================================================================== */

/**
 * Create timers from user input
 */
int create_timers(void)
{
    FIELD *field[5];
    FORM *form;

    field[0] = new_field(1, 30, 0, 13, 0, 0);
    field[1] = new_field(1, 30, 2, 13, 0, 0);
    field[2] = new_field(1, 30, 4, 13, 0, 0);
    field[3] = new_field(1, 30, 6, 13, 0, 0);
    field[4] = NULL;

    // Field options
    set_field_back(field[0], A_UNDERLINE);
    field_opts_off(field[0], O_AUTOSKIP);
    set_field_type(field[0], TYPE_REGEXP, "^([1-9][0-9]*[hms]?[\t ]*)+$");
    set_max_field(field[0], 31);

    set_field_back(field[1], A_UNDERLINE);
    field_opts_off(field[1], O_AUTOSKIP | O_STATIC);
    set_max_field(field[1], 63);

    set_field_back(field[2], A_UNDERLINE);
    field_opts_off(field[2], O_AUTOSKIP | O_STATIC);
    set_max_field(field[2], 63);

    set_field_back(field[3], A_UNDERLINE);
    field_opts_off(field[3], O_AUTOSKIP | O_STATIC);
    // Filename must have no spaces, unless enclosed within ""
    set_field_type(field[3], TYPE_REGEXP, "^(([^[:space:]]*)|(\".*\")) *$");
    set_max_field(field[3], 63);

    form = new_form(field);

    int h, w;
    getmaxyx(stdscr, h, w);

    int rows, cols;
    scale_form(form, &rows, &cols);

    boxwin = newwin(rows + 4, cols + 4, h/2 - rows/2 - 2, w/2 - cols/2 - 2);
    keypad(boxwin, TRUE);

    win = derwin(boxwin, rows, cols, 2, 2);

    set_form_win(form, boxwin);
    set_form_sub(form, win);

    WINDOW *popup = newwin(6, cols + 8, h/2 - 2, w/2 - cols/2 - 4);
    WINDOW *popup_content = derwin(popup, 3, cols + 4, 2, 2);

    // TODO:
    // - Determine why subsequent runs through the loop do not show the form
    //   attributes as expected (currently specifically underlined fields)
    // - Fix
    while (1) {
        box(boxwin, 0, 0);
        refresh();

        post_form(form);
        wprint_window_title(boxwin, "New Timer");
        wrefresh(boxwin);

        mvwprintw(win, 0, 0, "Duration:");
        mvwprintw(win, 2, 0, "Title:");
        mvwprintw(win, 4, 0, "Message:");
        mvwprintw(win, 6, 0, "Sound file:");
        wrefresh(win);

        print_status("Duration in s/m/h (default: s), e.g. '1m30s'. Press Enter to submit");

        form_driver(form, REQ_FIRST_FIELD);

        int ch;
        while((ch = wgetch(boxwin)) != '\n' || ch == KEY_ENTER) {
            switch(ch) {
                case KEY_DOWN:
                case '\t': {
                    form_driver(form, REQ_NEXT_FIELD);
                    // Go to the end of the existing buffer
                    form_driver(form, REQ_END_LINE);
                } break;

                case KEY_UP:
                case KEY_BTAB: {
                    form_driver(form, REQ_PREV_FIELD);
                    form_driver(form, REQ_END_LINE);
                } break;

                case KEY_LEFT: {
                    form_driver(form, REQ_LEFT_CHAR);
                } break;

                case KEY_RIGHT: {
                    form_driver(form, REQ_RIGHT_CHAR);
                } break;

                case KEY_BACKSPACE:
                case 127: {
                    form_driver(form, REQ_LEFT_CHAR);
                    form_driver(form, REQ_DEL_CHAR);
                    wrefresh(win);
                } break;

                default: {
                    // Print normal characters
                    form_driver(form, ch);
                } break;
            }
        }

        form_driver(form, REQ_VALIDATION);

        char *duration_input = strtrim(field_buffer(field[0], 0));

        // If no duration has been provided we quietly try again, avoiding
        // clearing the screen and clobbering any other form data
        int duration = parse_duration(duration_input);
        if (!duration) continue;

        char *title_input = strtrim(field_buffer(field[1], 0));
        char *msg_input = strtrim(field_buffer(field[2], 0));
        char *audio_input = strtrim(field_buffer(field[3], 0));
        char *title = NULL, *msg = NULL, *audio = NULL;

        // TODO: Separate into helper function
        if (title_input) {
            int len = strlen(title_input) + 1;
            title = malloc(len);
            strncpy(title, title_input, len);
        }
        if (msg_input) {
            int len = strlen(msg_input) + 1;
            msg = malloc(len);
            strncpy(msg, msg_input, len);
        }
        if (audio_input) {
            int len = strlen(audio_input) + 1;
            audio = malloc(len);
            strncpy(audio, audio_input, len);
        }

        Notification *notification = malloc(sizeof(Notification));
        *notification = (Notification) {.msg=msg, .aud=audio};

        add_timer(new_timer(.title=title, .duration=duration,
                    .on_display=display_time, .on_complete=notify, .data=notification));

        clear_status();

        // Clear form fields, except sound file
        // It is presumed that a user will generally want the same sound,
        // or might at least have sounds in the same directory and thus prefer
        // not to have to repeatedly type the whole path
        set_field_buffer(field[0], 0, "\0");
        set_field_buffer(field[1], 0, "\0");
        set_field_buffer(field[2], 0, "\0");

        box(popup, 0, 0);
        mvwprintw(popup_content, 0, 0, "Create another timer (y/N)? ");
        wrefresh(popup);
        char new = wgetch(popup_content);
        if (new != 'Y' && new != 'y') {
            wclear(popup);
            wrefresh(popup);
            break;
        }
        wclear(popup);
        wrefresh(popup);
    }

    // Clean up after form
    unpost_form(form);
    free_form(form);
    free_field(field[0]);
    free_field(field[1]);
    free_field(field[2]);
    free_field(field[3]);

    wclear(boxwin);
    // Remove the window title
    // TODO: Likely move to own function
    box(boxwin, 0, 0);
    wrefresh(boxwin);

    // TODO: Error handling
    return 0;
}

void print_title(char *title)
{
    box(boxwin, 0, 0);
    wprint_window_title(boxwin, title);
}

/**
 * Display window title text
 * Expects title to be null-terminated
 */
void wprint_window_title(WINDOW *w, char *t)
{
    if (!*t) return;
    mvwprintw(w, 0, 1, " %s ", t);
    wrefresh(w);
    refresh();
}

void wprint_status(WINDOW *w, char *s)
{
    int y, x;
    getyx(w, y, x);
    int status_y = getmaxy(w);
    mvwprintw(w, status_y - 2, 1, "%s", s);
    clrtoeol();
    wmove(w, y, x);
    wrefresh(w);
}

void wclear_status(WINDOW *w)
{
    int y, x;
    getyx(w, y, x);
    int status_y = getmaxy(w);
    wmove(w, status_y - 2, 0);
    clrtoeol();
    wmove(w, y, x);
    wrefresh(w);
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
        if(t.title) free(t.title);
        if(t.data) {
            Notification *n = t.data;
            free(n->msg);
            free(n->aud);
            free(t.data);
        }
    }
    return 0;
}


/* ========================================================================== */
/* Helper Functions - should likely be separated out into their own file      */
/* ========================================================================== */

/**
 * Left trim string
 * We do not alter the provided string, or allocate any new memory
 * We simply return the a pointer to the first non-whitespace char
 */
char *strtriml(char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '\n')
        s++;
    return s;
}

/**
 * Right trim string
 * We /do/ potentially alter the provided string by adding a nul
 */
char *strtrimr(char *s)
{
    int len = strlen(s);
    if (!len) return s;
    char *e = s + len - 1;
    while (*e == ' ' || *e == '\t' || *e == '\n')
        e--;
    *(e + 1) = '\0';
    return s;
}

/**
 * String trim
 * Trim both left and right trailing whitespace
 */
char *strtrim(char *s)
{
    return strtrimr(strtriml(s));
}
