#include <alsa/asoundlib.h>
#include "SignalTree.h"
#include "SignalPinkNoise.h"

int main_alsa(snd_pcm_t *handle) {
    SignalTree *tree = SignalTree_Init(1);
    Signal *pink = Signal_InitPinkNoise();
    SignalTree_AddSignal(tree, 0, pink);

    while(1) {
        int err;
        SignalTree_Process(tree);
        err = snd_pcm_writei(handle, pink->output, SIGNAL_BUFFER_SIZE);
        if (err < 0) {
            printf("snd_pcm_writei error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
        }
    }
}
