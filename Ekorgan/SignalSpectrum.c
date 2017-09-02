#include "SignalSpectrum.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define M_PI 3.14159265358979323846

typedef struct SpectrumState {
	index_t n_table;
    index_t n_comp;
	float *table;
    float *freq;
	float *amp;
	float *phase;
} SpectrumState;

static void process(Signal *signal) {
	SpectrumState *state = signal->state;
	for (index_t i = 0; i < SIGNAL_BUFFER_SIZE; i++) {
        signal->output[i] = 0.0;
        for (index_t j = 0; j < state->n_comp; j++) {
            index_t index = state->phase[j] * state->n_table;
            signal->output[i] += state->table[index] * state->amp[j];
            state->phase[j] += state->freq[j] / SAMPLE_RATE;
            if (state->phase[j] >= 1.0f) {
                state->phase[j] -= 1.0f;
            }
        }
	}
}

static void free_state(SpectrumState *state) {
	free(state->table);
	free(state);
}

Signal *Signal_InitSpectrum(index_t n_table, index_t n_comp, float *table, float *freq, float *amp, float *phase, bool take_references) {
	SpectrumState *state = malloc(sizeof(SpectrumState));
	state->n_table = n_table;
    state->n_comp= n_comp;
	if (take_references) {
		state->table = table;
        state->freq = freq;
        state->amp = amp;
        state->phase = phase;
	}
	else {
		state->table = malloc(sizeof(float) * n_table);
        state->freq = malloc(sizeof(float) * n_comp);
        state->amp = malloc(sizeof(float) * n_comp);
        state->phase = malloc(sizeof(float) * n_comp);
		memcpy(state->table, table, sizeof(float) * n_table);
        memcpy(state->freq, freq, sizeof(float) * n_comp);
        memcpy(state->amp, amp, sizeof(float) * n_comp);
        memcpy(state->phase, phase, sizeof(float) * n_comp);
	}
	return Signal_InitBase(1, state, &process, &free_state);
}

Signal *Signal_InitSineSpectrum(index_t n_table, index_t n_comp, index_t n_waves, index_t *relative_freqs, float *relative_amps, float *freq, float *amp, float *phase) {
	float *table = malloc(sizeof(float) * n_table);
	for (index_t i = 0; i < n_table; i++) {
		table[i] = 0.;
		for (index_t j = 0; j < n_waves; j++) {
			table[i] += sinf(2 * (float)M_PI * i * relative_freqs[j] / n_table) * relative_amps[j];
		}
	}
	Signal *signal = Signal_InitSpectrum(n_table, n_comp, table, freq, amp, phase, false);
    free(table);
    return signal;
}
