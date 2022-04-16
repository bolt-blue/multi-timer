/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef AUDIO_H
#define AUDIO_H

// TODO: Move to utils header
#include <stdbool.h>

#include <portaudio.h>
#include <sndfile.h>

// TODO: Make these errors independent of the library being used
typedef enum {
   ERR_LOAD_AUDIO = 1,
   ERR_PA_INIT,
   ERR_PA_DEVICE,
   ERR_PA_STREAM_OPEN,
   ERR_PA_STREAM_START,
} AudioError;

/**
 * Must be provided by all implementations
 * `play_audio()` is required to be non-blocking
 */
bool mtm_is_audio_file(char *filename);
int mtm_play_audio(char *file);
int mtm_stop_audio(void);
const char *mtm_audioerr(void);

#endif /* AUDIO_H */
