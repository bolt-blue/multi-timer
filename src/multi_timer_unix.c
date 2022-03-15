#include <stdio.h>
#include <time.h>

#include "multi_timer.h"

#define ONE_TENTH_SEC 1000000000L / 10

/**
 * Initialise the countdown
 * Once complete, if a callback is present, run it then return
 */
void run_timer(mtimer_t t)
{
    // TODO:
    // - Separate out display functions (e.g. `display_time`)
    //   - These should be handled by CLI/GUI layer, per platform
    // - Fork this process?
    //   Do we care if this blocks the application?

    // TODO: Generate error, or set default
    if (!t.display) return;

    struct timespec wait = (struct timespec){.tv_nsec=ONE_TENTH_SEC};
    struct timespec begin;
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &begin);

    time_t delta, counter = 0;
    time_t remaining = t.duration;

    t.display(remaining);
    while (remaining) {
        nanosleep(&wait, NULL);
        clock_gettime(CLOCK_MONOTONIC, &now);

        delta = now.tv_sec - begin.tv_sec;
        if (delta > counter && now.tv_nsec >= begin.tv_nsec) {
            counter++;
            remaining--;
            t.display(remaining);
        }
    }

    if (t.on_complete) t.on_complete(t.data);
}
