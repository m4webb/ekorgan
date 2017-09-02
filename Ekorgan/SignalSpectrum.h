/*
Filename:
	SignalSpectrum.h
*/

#pragma once

#include "SignalTree.h"
#include <stdbool.h>

Signal *Signal_InitSpectrum(index_t n_table, index_t n_comp, float *table, float *freq, float *amp, float *phase, bool take_references);
Signal *Signal_InitSineSpectrum(index_t n_table, index_t n_comp, index_t n_waves, index_t *relative_freqs, float *relative_amps, float *freq, float *amp, float *phase);
