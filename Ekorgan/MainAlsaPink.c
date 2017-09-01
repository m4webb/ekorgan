#include <alsa/asoundlib.h>
#include "SignalTree.h"
#include "SignalPinkNoise.h"

int main(int argc, char *argv[]) {

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hwparams;

    snd_pcm_open(&handle, "hw:0,0", SND_PCM_STREAM_PLAYBACK, 0);

    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_hw_params_any(handle, hwparams);
    snd_pcm_hw_params_set_access(handle, hwparams,
            SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, hwparams, SND_PCM_FORMAT_FLOAT);
    snd_pcm_hw_params_set_channels(handle, hwparams, 1);
    snd_pcm_hw_params_set_rate(handle, hwparams, SAMPLE_RATE, 0);
    snd_pcm_hw_params_set_buffer_size(handle, hwparams, SIGNAL_BUFFER_SIZE);

    snd_pcm_hw_params(handle, hwparams);

    SignalTree *tree = SignalTree_Init(1);
    Signal *pink = Signal_InitPinkNoise();
    SignalTree_AddSignal(tree, 0, pink);

    while(1) {
        SignalTree_Process(tree);
        snd_pcm_writei(handle, pink->output, SIGNAL_BUFFER_SIZE);
    }
}
