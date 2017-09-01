/*
Filename:
	SignalMultiPhaseTable.h

Description:
	Caches a table and cycles through it repeatedly at the frequencies given by
	the inputs, linearly interpolating the values.

Signal inputs:
	1...n Frequencies
*/

#pragma once

#include <SignalTree.h>
#include <stdbool.h>

Signal *Signal_InitMultiPhaseTable(index_t n_phases, float *freqs, bool take_freqs_reference, index_t n_table, float *table, bool take_reference, float mult, float add);
Signal *Signal_InitMultiSineTable(index_t n_phases, float *freqs, bool take_freqs_reference, index_t n_table, index_t n_waves, index_t *relative_freqs, float *relative_amps, float mult, float add);