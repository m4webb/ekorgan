#include <SignalLinearCombination.h>
#include <stdlib.h>
#include <string.h>

typedef struct LinearCombinationState {
	index_t n_weights;
	float *weights;
} LinearCombinationState;

static void process(Signal *signal) {
	LinearCombinationState *state = signal->state;
	for (index_t i = 0; i < SIGNAL_BUFFER_SIZE; i++) {
		signal->output[i] = 0.0;
		for (index_t j = 0; j < state->n_weights; j++) {
			signal->output[i] += state->weights[j] * signal->input[j][i];
		}
	}
}

static void free_state(LinearCombinationState *state) {
	free(state->weights);
	free(state);
}

Signal *Signal_InitLinearCombination(index_t n_weights, float *weights, bool take_reference) {
	LinearCombinationState *state = malloc(sizeof(LinearCombinationState));
	state->n_weights = n_weights;
	if (take_reference) {
		state->weights = weights;
	}
	else {
		state->weights = malloc(sizeof(float) * n_weights);
		memcpy(state->weights, weights, sizeof(float) * n_weights);
	}
	return Signal_InitBase(n_weights, state, &process, &free_state);
}