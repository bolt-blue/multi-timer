/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef MTM_H
#define MTM_H

#include "multi_timer.h"

typedef struct {
    char *msg;  /* Message string */
    char *aud;  /* Filepath to audio file */
} Notification;

// Forward declarations - Implemented by UI
int init_ui(void);
void teardown_ui(void);
int run(void);
void display_time(long seconds);
void notify(void *data);

#endif /* MTM_H */
