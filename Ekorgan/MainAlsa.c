#include <alsa/asoundlib.h>
#include "SignalTree.h"

int main_alsa(snd_pcm_t *handle);

int main(int argc, char *argv[]) {

    int err;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hwparams;

    snd_pcm_hw_params_alloca(&hwparams);

    err = snd_pcm_open(&handle, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        printf("snd_pcm_open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_any(handle, hwparams);
    if (err < 0) {
        printf("snd_pcm_hw_params_any error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_access(handle, hwparams,
            SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        printf("snd_pcm_hw_params_set_access error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_FLOAT);
    if (err < 0) {
        printf("snd_pcm_hw_params_set_format error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_channels(handle, hwparams, 1);
    if (err < 0) {
        printf("snd_pcm_hw_params_set_channels error: %s\n",
                snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_rate(handle, hwparams, SAMPLE_RATE, 0);
    if (err < 0) {
        printf("snd_pcm_hw_params_set_rate error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_buffer_size(handle, hwparams,
            SIGNAL_BUFFER_SIZE);
    if (err < 0) {
        printf("snd_pcm_hw_params_set_buffer_size error: %s\n",
                snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params(handle, hwparams);
    if (err < 0) {
        printf("snd_pcm_hw_params error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    return main_alsa(handle);
}
