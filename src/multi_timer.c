#include "multi_timer.h"

#include <time.h>
#include <stdint.h>
#include <stdio.h>

#define ONE_TENTH_SEC 1000000000L / 10

void display_time(long s);

void run_timer(mtimer_t t)
{
    // TODO:
    // - Separate out display functions (e.g. `display_time`)
    //   - These should be handled by CLI/GUI layer, per platform
    // - Fork this process?
    //   Do we care if this blocks the application?

    struct timespec wait = (struct timespec){.tv_nsec=ONE_TENTH_SEC};
    struct timespec begin;
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &begin);

    time_t delta, counter = 0;
    time_t remaining = t.duration;

    display_time(remaining);
    while (remaining) {
        nanosleep(&wait, NULL);
        clock_gettime(CLOCK_MONOTONIC, &now);

        delta = now.tv_sec - begin.tv_sec;
        if (delta > counter && now.tv_nsec >= begin.tv_nsec) {
            counter++;
            remaining--;
            display_time(remaining);
        }
    }
    putc('\n', stdout);

    if (t.on_complete) t.on_complete(t.data);
}

void display_time(long seconds)
{
    int32_t h = seconds / 60 / 60;
    int32_t m = (seconds - h * 60 * 60) / 60;
    int32_t s = seconds - h * 60 * 60 - m * 60;
    printf("\r%02d:%02d:%02d", h, m, s);
    fflush(stdout);
}
