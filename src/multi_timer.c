#include "multi_timer.h"

#include <ctype.h>
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

/**
 * Convert duration string into integer seconds
 * Duration d is presumed to be nul-terminated
 * Duration d is presumed to have only plain digits, or digits followed by one
 * of the suffixes 's', 'm' or 'h'
 * Return duration in seconds
 */
int parse_duration(char *dstr)
{
    int s = 0;

    int tmp_sum = 0;
    char *cur = dstr;
    while (1) {
        if (!isdigit(*cur)) {
            char type = *cur;

            *cur = '\0';
            tmp_sum += atoi(dstr);

            if (type == 'm')
                tmp_sum *= 60;
            else if (type == 'h')
                tmp_sum *= 60 * 60;

            s += tmp_sum;

            if (type == '\0')
                break;
            dstr = cur + 1;
            tmp_sum = 0;
        }
        cur++;
    }

    return s;
}
