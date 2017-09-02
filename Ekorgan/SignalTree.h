/*
Filename:
	SignalTree.h

Description:
	The core signal tree library.
*/

#pragma once

#include <stdint.h>

#define SAMPLE_RATE 22050
#define SIGNAL_BUFFER_SIZE 8192

typedef uint16_t index_t;
typedef struct Signal Signal;
typedef struct SignalTree SignalTree;
typedef void SignalProcessFunction(Signal *);
typedef void SignalFreeStateFunction(void *);

typedef struct Signal {
	index_t n_input;
	const float **input;
	float *output;
	void *state;
	SignalProcessFunction *process;
	SignalFreeStateFunction *free_state;
} Signal;

typedef struct SignalTree {
	index_t n;
	Signal **signals;
} SignalTree;

Signal *Signal_InitBase(index_t n_input, void *state, SignalProcessFunction *process, SignalFreeStateFunction *free_state);
void Signal_Free(Signal *signal);

SignalTree *SignalTree_Init(index_t n_signals);
void SignalTree_Free(SignalTree *tree);
void SignalTree_FreeAll(SignalTree *tree);
void SignalTree_AddSignal(SignalTree *tree, index_t index, Signal *signal);
void SignalTree_Connect(SignalTree *tree, index_t from_index, index_t to_index, index_t input_index);
void SignalTree_Process(SignalTree *tree);
