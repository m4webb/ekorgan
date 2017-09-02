#include <alsa/asoundlib.h>
#include "SignalTree.h"
#include "SignalSpectrum.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N_COMP 75 
#define N_WAVE 20
#define N_TABLE 16384

static int xrun_recovery(snd_pcm_t *handle, int err)
{
        printf("stream recovery\n");
        if (err == -EPIPE) {    /* under-run */
                err = snd_pcm_prepare(handle);
                if (err < 0)
                        printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
                return 0;
        } else if (err == -ESTRPIPE) {
                while ((err = snd_pcm_resume(handle)) == -EAGAIN)
                        sleep(1);       /* wait until the suspend flag is released */
                if (err < 0) {
                        err = snd_pcm_prepare(handle);
                        if (err < 0)
                                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
                }
                return 0;
        }
        return err;
}

float randf() {
	return (float)rand() / (float)RAND_MAX;
}

int main_alsa(snd_pcm_t *handle) {
    int i;
    index_t *relative_freqs = malloc(sizeof(index_t) * N_WAVE);
    float *relative_amps = malloc(sizeof(float) * N_WAVE);
    for (i = 0; i < N_WAVE; i++) {
        relative_freqs[i] = i + 1;
        relative_amps[i] = sqrtf(1.0f / (i + 1));
    }
    float *freq = malloc(sizeof(float) * N_COMP);
    float *amp = malloc(sizeof(float) * N_COMP);
    float *phase = malloc(sizeof(float) * N_COMP);
    srand(time(NULL));

    float sum_amp = 0.0f;
    for (i = 0; i < N_COMP; i++) {
        freq[i] = expf(randf()*4.0f + 3.0f);
        amp[i] = sqrtf(1.0f / freq[i]);
        sum_amp += amp[i];
        phase[i] = (float) i / N_COMP;
    }
    
    for (i = 0; i < N_COMP; i++) {
        amp[i] /= sum_amp;
        amp[i] *= 2.0f;
        printf("freq[%d] = %f, amp[%d] = %f\n", i, freq[i], i, amp[i]); 
    }

    SignalTree *tree = SignalTree_Init(1);
    Signal *spectrum = Signal_InitSineSpectrum(N_TABLE, N_COMP, N_WAVE,
            relative_freqs, relative_amps, freq, amp, phase);
    SignalTree_AddSignal(tree, 0, spectrum);

    while(1) {
        int err;
        SignalTree_Process(tree);
        err = snd_pcm_writei(handle, spectrum->output, SIGNAL_BUFFER_SIZE);
        if (err == -EAGAIN) {
            printf("Unexpected EAGAIN\n");
            continue;
        }
        if (err < 0) {
            if (xrun_recovery(handle, err) < 0) {
                printf("Write error: %s\n", snd_strerror(err));
                exit(EXIT_FAILURE);
            }
        }
    }
}
