/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MTM_H
#define MTM_H

#include "multi_timer.h"

// Forward declarations - Implemented by UI
int init_ui(void);
void teardown_ui(void);
int run(void);
void display_time(long seconds);
// TODO: Remove from here to allow an implementation to provide
// multiple options for notifications
void notify(void *data);

#endif /* MTM_H */
