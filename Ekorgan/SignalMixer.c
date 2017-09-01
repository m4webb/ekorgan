#include <SignalMixer.h>

static void process(Signal *signal) {
	for (index_t i = 0; i < SIGNAL_BUFFER_SIZE; i++) {
		signal->output[i] = 0.0;
		for (index_t j = 0; j < signal->n_input; j++) {
			signal->output[i] += signal->input[j][i];
		}
	}
}

static void free_state(void *state) {
	return;
}

Signal *Signal_InitMixer(index_t n_input) {
	return Signal_InitBase(n_input, NULL, &process, &free_state);
}