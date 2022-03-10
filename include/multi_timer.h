/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MULTI_TIMER_H
#define MULTI_TIMER_H

#include <stdint.h>

/**
 * Facilitate the creation of callback functions
 */
typedef void (*tmr_cb)(void *);

/**
 * A countdown timer is a duration in seconds, along with an optional
 * callback function and custom data
 */
typedef struct Timer {
    uint32_t duration;      // In seconds
    tmr_cb on_complete;     // Callback
    void *data;             // Data passed to callback
} mtimer_t;

/**
 * Helper macro to simplify the creation of timer structs
 */
#define new_timer(...) (mtimer_t){__VA_ARGS__}

/**
 * Initialise the countdown
 * Once complete, if a callback is present, run it then return
 */
void run_timer(mtimer_t t);

#endif /* MULTI_TIMER_H */
