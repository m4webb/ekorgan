#include <SignalPhaseTable.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define M_PI 3.14159265358979323846

typedef struct PhaseTableState {
	index_t n_table;
	float *table;
	float phase;
	float mult;
	float add;
} PhaseTableState;

static void process(Signal *signal) {
	PhaseTableState *state = signal->state;
	for (index_t i = 0; i < SIGNAL_BUFFER_SIZE; i++) {
		index_t index0 = (index_t)(state->phase * state->n_table);
		index_t index1 = (index0 + 1);
		while (index0 >= state->n_table) {
			index0 -= state->n_table;
		}
		while (index1 >= state->n_table) {
			index1 -= state->n_table;
		}
		float weight0 = 1.0f - (state->phase * state->n_table - index0);
		float weight1 = 1.0f - weight0;
		float interpolation = weight0 * state->table[index0] + weight1 * state->table[index1];
		signal->output[i] = interpolation*state->mult + state->add;
		state->phase += signal->input[0][i] / SAMPLE_RATE;
		while (state->phase >= 1.0f) {
			state->phase -= 1.0f;
		}
	}
}

static void free_state(PhaseTableState *state) {
	free(state->table);
	free(state);
}

Signal *Signal_InitPhaseTable(index_t n_table, float *table, bool take_reference, float mult, float add, float phase) {
	PhaseTableState *state = malloc(sizeof(PhaseTableState));
	state->n_table = n_table;
	if (take_reference) {
		state->table = table;
	}
	else {
		state->table = malloc(sizeof(float) * n_table);
		memcpy(state->table, table, sizeof(float) * n_table);
	}
	state->phase = phase;
	state->mult = mult;
	state->add = add;
	return Signal_InitBase(1, state, &process, &free_state);
}

Signal *Signal_InitSineTable(index_t n_table, index_t n_waves, index_t *relative_freqs, float *relative_amps, float mult, float add, float phase) {
	float *table = malloc(sizeof(float) * n_table);
	for (index_t i = 0; i < n_table; i++) {
		table[i] = 0.;
		for (index_t j = 0; j < n_waves; j++) {
			table[i] += sinf(2 * (float)M_PI * i * relative_freqs[j] / n_table) * relative_amps[j];
		}
	}
	return Signal_InitPhaseTable(n_table, table, true, mult, add, phase);
}

Signal *Signal_InitGADSRTable(index_t n_sections, index_t *ratios, float *levels) {
	index_t n_table = 0;
	for (index_t i = 0; i < n_sections; i++) {
		n_table += ratios[i];
	}
	float *table = malloc(sizeof(float) * n_table);
	index_t index = 0;
	for (index_t i = 0; i < n_sections; i++) {
		for (index_t j = 0; j < ratios[i]; j++) {
			float weight0 = 1.0f - (float)j / ratios[i];
			float weight1 = 1.0f - weight0;
			assert(weight0 >= 0.0f);
			assert(weight0 <= 1.0f);
			assert(weight1 >= 0.0f);
			assert(weight1 <= 1.0f);
			table[index++] = weight0 * levels[i] + weight1 * levels[i + 1];
		}
	}
	return Signal_InitPhaseTable(n_table, table, true, 1.0f, 0.0f, 0.0f);
}