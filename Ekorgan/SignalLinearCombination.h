/*
Filename:
	SignalLinearCombination.h

Description:
	Linear comination of input streams with fixed weights.

Signal Inputs:
	1...n Inputs
*/

#pragma once

#include <SignalTree.h>
#include <stdbool.h>

Signal *Signal_InitLinearCombination(index_t n_weights, float *weights, bool take_reference);