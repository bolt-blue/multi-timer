#include <stdint.h>
#include <stdio.h>

/**
 * Display timer to user
 */
void display_time(long seconds)
{
    int32_t h = seconds / 60 / 60;
    int32_t m = (seconds - h * 60 * 60) / 60;
    int32_t s = seconds - h * 60 * 60 - m * 60;
    printf("\r%02d:%02d:%02d", h, m, s);
    fflush(stdout);
}

/**
 * Take a message string and print to stdout
 * Has to accept void * in order to be compatible with multi timer callback
 */
void notify(void *message)
{
    if (!message) return;
    char *msg = (char *)message;
    printf("\n%s\n", msg);
}
