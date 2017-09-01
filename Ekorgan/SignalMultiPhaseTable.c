#include <SignalMultiPhaseTable.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define M_PI 3.14159265358979323846

typedef struct MultiPhaseTableState {
	index_t n_table;
	float *table;
	index_t n_phases;
	float *phases;
	float *freqs;
	float mult;
	float add;
} MultiPhaseTableState;

static void process(Signal *signal) {
	MultiPhaseTableState *state = signal->state;
	for (index_t i = 0; i < SIGNAL_BUFFER_SIZE; i++) {
		signal->output[i] = 0.0;
		for (index_t k = 0; k < state->n_phases; k++) {
			index_t index0 = (index_t)(state->phases[k] * state->n_table);
			index_t index1 = (index0 + 1);
			while (index0 >= state->n_table) {
				index0 -= state->n_table;
			}
			while (index1 >= state->n_table) {
				index1 -= state->n_table;
			}
			float weight0 = 1.0f - (state->phases[k] * state->n_table - index0);
			float weight1 = 1.0f - weight0;
			float interpolation = weight0 * state->table[index0] + weight1 * state->table[index1];
			signal->output[i] += interpolation*state->mult + state->add;
			assert(signal->output[i] <= 1.0f);
			assert(signal->output[i] >= -1.0f);
			state->phases[k] += state->freqs[k] / SAMPLE_RATE;
			while (state->phases[k] >= 1.0f) {
				state->phases[k] -= 1.0f;
			}
		}
	}
}

static void free_state(MultiPhaseTableState *state) {
	free(state->table);
	free(state->phases);
	free(state);
}

Signal *Signal_InitMultiPhaseTable(index_t n_phases, float *freqs, bool take_freqs_reference, index_t n_table, float *table, bool take_reference, float mult, float add) {
	MultiPhaseTableState *state = malloc(sizeof(MultiPhaseTableState));
	state->n_table = n_table;
	if (take_reference) {
		state->table = table;
	}
	else {
		state->table = malloc(sizeof(float) * n_table);
		memcpy(state->table, table, sizeof(float) * n_table);
	}
	state->n_phases = n_phases;
	state->phases = malloc(sizeof(float) * n_phases);
	for (int k = 0; k < n_phases; k++) {
		state->phases[k] = (float)rand() / (float)RAND_MAX;
	}
	if (take_freqs_reference) {
		state->freqs = freqs;
	}
	else {
		state->freqs = malloc(sizeof(float) * n_phases);
		memcpy(state->freqs, freqs, sizeof(float) * n_phases);
	}
	state->mult = mult;
	state->add = add;
	return Signal_InitBase(0, state, &process, &free_state);
}

Signal *Signal_InitMultiSineTable(index_t n_phases, float *freqs, bool take_freqs_reference, index_t n_table, index_t n_waves, index_t *relative_freqs, float *relative_amps, float mult, float add) {
	float *table = malloc(sizeof(float) * n_table);
	for (index_t i = 0; i < n_table; i++) {
		table[i] = 0.;
		for (index_t j = 0; j < n_waves; j++) {
			table[i] += sinf(2 * (float)M_PI * i * relative_freqs[j] / n_table) * relative_amps[j];
		}
	}
	return Signal_InitMultiPhaseTable(n_phases, freqs, take_freqs_reference, n_table, table, true, mult, add);
}