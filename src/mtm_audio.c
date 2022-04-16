// These are both for a workaround to avoid portaudio dumping text to stderr
#include <fcntl.h>
#include <unistd.h>

#include "mtm_audio.h"

/* References:
 * https://libsndfile.github.io/libsndfile/api.html
 * https://github.com/hosackm/wavplayer/blob/master/src/wavplay.c
 */

// TODO: Move to utils header
#define internal static

#define PA_SAMPLE_TYPE paFloat32
typedef int SAMPLE;

typedef struct {
    SNDFILE *file;
    SF_INFO info;
} audio_data;

internal PaStream *stream;
internal PaError pa_err;
internal audio_data cb_data;

internal int pa_callback(const void *input_buffer, void *output_buffer,
        unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags, void *user_data);

bool mtm_is_audio_file(char *filename)
{
    SNDFILE *tmp;
    SF_INFO tmp_info;
    if (!(tmp = sf_open(filename, SFM_READ, &tmp_info)))
        return false;
    sf_close(tmp);
    return true;
}

int mtm_play_audio(char *filename)
{
    // sndfile
    if ((cb_data.file = sf_open(filename, SFM_READ, &cb_data.info)) == NULL) {
        return ERR_LOAD_AUDIO;
    }

    int seconds = 1;
    int frames_per_buffer = seconds * cb_data.info.samplerate;

    pa_err = Pa_Initialize();
    if (pa_err != paNoError) {
        mtm_stop_audio();
        return ERR_PA_INIT;
    }

    PaStreamParameters output_parameters;

    output_parameters.device = Pa_GetDefaultOutputDevice();
    if (output_parameters.device == paNoDevice) {
        // TODO: Do not print; just store an error reference
        fprintf(stderr, "Error: No default output device.\n");
        mtm_stop_audio();
        return ERR_PA_DEVICE;
    }

    output_parameters.channelCount = cb_data.info.channels;
    output_parameters.sampleFormat = PA_SAMPLE_TYPE;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = NULL;

    pa_err = Pa_OpenStream(&stream, NULL, &output_parameters,
            cb_data.info.samplerate, frames_per_buffer,
            paClipOff, pa_callback, &cb_data);
    if (pa_err != paNoError) {
        mtm_stop_audio();
        return ERR_PA_STREAM_OPEN;
    }

    pa_err = Pa_StartStream(stream);
    if (pa_err != paNoError) {
        mtm_stop_audio();
        return ERR_PA_STREAM_START;
    }

    return 0;
}

int mtm_stop_audio(void)
{
    if (stream)
        Pa_StopStream(stream);
    pa_err = Pa_Terminate();
    if (pa_err != paNoError)
        // TODO: Do not print; just store an error reference
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(pa_err));
    sf_close(cb_data.file);

    return 0;
}

/**
 * PortAudio callback
 * Called by the PA engine when audio is needed
 * May be called at the interrupt level, so don't do anything that could mess
 * up the system like calling malloc() or free()
 * Ref: http://files.portaudio.com/docs/v19-doxydocs/writing_a_callback.html
 */
int pa_callback(const void *input_buffer, void *output_buffer,
        unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags, void *user_data)
{
    audio_data *data = user_data;
    float *out = output_buffer;

    // TODO: Better approach to looping?
    int res = sf_read_float(data->file, out, frames_per_buffer * data->info.channels);
    if (!res) {
        sf_seek(data->file, 0, SEEK_SET);
        sf_read_float(data->file, out, frames_per_buffer * data->info.channels);
    }

    return 0;
}
