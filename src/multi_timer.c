#include "multi_timer.h"

#include <stdlib.h>

/*
 * Platform-specific entry point
 */
#ifdef __unix__
#include "multi_timer_unix.c"

#elif __APPLE__
// TODO: OS X platform layer

#elif _WIN32
// TODO: Windows platform layer

#endif

static mtimer_t *timers_g;
static int tcount_g;

int add_timer(mtimer_t t)
{
    // TODO: Error handling
    timers_g = realloc(timers_g, ++tcount_g * sizeof(mtimer_t));
    timers_g[tcount_g-1] = t;
    return 0;
}

void free_timers(void)
{
    // We make no assumptions about the internals of each timer struct;
    // they are the responsibility of the caller.
    free(timers_g);
}

int num_timers(void)
{
    return tcount_g;
}

mtimer_t get_timer(int i)
{
    return timers_g[i];
}
