#include <SignalConstant.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct ConstantState {
	float value;
	bool dirty;
} ConstantState;

static void process(Signal *signal) {
	ConstantState *state = signal->state;
	if (!state->dirty) {
		return;
	}
	for (index_t i = 0; i < SIGNAL_BUFFER_SIZE; i++) {
		signal->output[i] = state->value;
	}
	state->dirty = false;
}

static void free_state(ConstantState *state) {
	free(state);
}

Signal *Signal_InitConstant(float value) {
	ConstantState *state = malloc(sizeof(ConstantState));
	state->value = value;
	state->dirty = true;
	return Signal_InitBase(0, state, &process, &free_state);
}

void Signal_SetConstant(Signal *signal, float value) {
	ConstantState *state = signal->state;
	state->value = value;
	state->dirty = true;
}