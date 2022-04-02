/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MULTI_TIMER_H
#define MULTI_TIMER_H

#include <stdint.h>

/**
 * A countdown timer is a duration in seconds, along with an optional
 * callback function and custom data
 */
typedef struct Timer {
    uint32_t duration;              // In seconds
    char *title;                    // Timer title/description
    void (*on_display)(long);       // Display function
    void (*on_complete)(void *);    // Callback function
    void *data;                     // Data passed to callback
} mtimer_t;

/**
 * Helper macro to simplify the creation of timer structs
 */
#define new_timer(...) (mtimer_t){__VA_ARGS__}

// Timer memory is handled internally - platform independent
int add_timer(mtimer_t timer);
void free_timers(void);
int num_timers(void);
mtimer_t get_timer(int index);

// Each platform will need to provide the following
// See: 'multi_timer_<platform>.c'
void run_timer(mtimer_t t);

#endif /* MULTI_TIMER_H */
