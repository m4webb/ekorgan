#include <SignalAttenuator.h>

static void process(Signal *signal) {
	for (index_t i = 0; i < SIGNAL_BUFFER_SIZE; i++) {
		signal->output[i] = signal->input[0][i];
		signal->output[i] *= signal->input[1][i];
	}
}

static void free_state(void *state) {
	return;
}

Signal *Signal_InitAttenuator() {
	return Signal_InitBase(2, NULL, &process, &free_state);
}
