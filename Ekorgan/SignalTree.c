#include "SignalTree.h"
#include <assert.h>
#include <stdlib.h>

Signal *Signal_InitBase(index_t n_input, void *state, SignalProcessFunction *process, SignalFreeStateFunction *free_state) {
	Signal *signal = malloc(sizeof(Signal));
	signal->n_input = n_input;
	signal->input = malloc(sizeof(float *) * n_input);
	signal->output = malloc(sizeof(float) * SIGNAL_BUFFER_SIZE);
	signal->state = state;
	signal->process = process;
	signal->free_state = free_state;
	return signal;
}

void Signal_Free(Signal *signal) {
	(*signal->free_state)(signal->state);
	free(signal->input);
	free(signal->output);
	free(signal);
}

SignalTree *SignalTree_Init(index_t n) {
	SignalTree *tree = malloc(sizeof(SignalTree));
	tree->n = n;
	tree->signals = malloc(sizeof(Signal *) * n);
	return tree;
}

void SignalTree_Free(SignalTree *tree) {
	free(tree->signals);
	free(tree);
}

void SignalTree_FreeAll(SignalTree *tree) {
	for (index_t i = 0; i < tree->n; i++) {
		Signal_Free(tree->signals[i]);
	}
	SignalTree_Free(tree);
}

void SignalTree_AddSignal(SignalTree *tree, index_t index, Signal *signal) {
	assert(index < tree->n);
	tree->signals[index] = signal;
}

void SignalTree_Connect(SignalTree *tree, index_t from_index, index_t to_index, index_t input_index) {
	assert(from_index < to_index);
	Signal *from_signal = tree->signals[from_index];
	Signal *to_signal = tree->signals[to_index];
	assert(input_index < to_signal->n_input);
	to_signal->input[input_index] = from_signal->output;
}

void SignalTree_Process(SignalTree *tree) {
	for (index_t i = 0; i < tree->n; i++) {
		Signal *signal = tree->signals[i];
		(*signal->process)(signal);
	}
}
