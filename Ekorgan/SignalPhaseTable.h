/*
Filename:
	SignalPhaseTable.h

Description:
	Caches a table and cycles through it repeatedly at the frequency given by
	the input, linearly interpolating the values.

Signal inputs:
	1. Frequency
*/

#pragma once

#include <SignalTree.h>
#include <stdbool.h>

Signal *Signal_InitPhaseTable(index_t n, float *table, bool take_reference, float mult, float add, float phase);
Signal *Signal_InitSineTable(index_t n_table, index_t n_waves, index_t *relative_freqs, float *relative_amps, float mult, float add, float phase);
Signal *Signal_InitGADSRTable(index_t n_sections, index_t *ratios, float *levels);