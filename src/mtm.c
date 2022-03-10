#include <stdio.h>

#include "multi_timer.h"

// Forward declarations
void notify(void *);

int main(int argc, char **argv)
{
    mtimer_t first_timer = new_timer(.duration=3, .on_complete=notify, .data="Ding! We're done.");

    run_timer(first_timer);

    return 0;
}

/**
 * Take a message string and print to stdout
 * Has to accept void * in order to be compatible with multi timer callback
 */
void notify(void *message)
{
    if (!message) return;
    char *msg = (char *)message;
    printf("%s\n", msg);
}
