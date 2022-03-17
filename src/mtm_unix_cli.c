#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Display timer title text
 * Expects title to be null-terminated
 */
void print_title(char *title)
{
    printw("%s\n", title);
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
    printw("\n%s\nPress any key to continue...", msg);
    getch();
    clear();
}
