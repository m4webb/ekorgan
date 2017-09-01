#define _CRT_SECURE_NO_DEPRECATE

#include <SignalTree.h>
#include <SignalConstant.h>
#include <SignalMixer.h>
#include <SignalPhaseTable.h>
#include <SignalLinearCombination.h>
#include <SDL.h>
#include <memory.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define N_NODES 4
#define NODES_START 4
#define N_SYNTHS_PER_NODE 50
#define N_SIGNALS N_NODES * N_SYNTHS_PER_NODE * 2 + 5
#define INDEX_MIXER N_SIGNALS - 1
#define INDEX_ROOT 1
#define INDEX_SIGMA 3
#define INDEX_SYNTHS_START 4

#define NN 1000

SignalTree *tree;
FILE *out;

void fill_audio(void *udata, Uint8 *stream, int len);
void init_synths();
void init_synths2();
void free_synths();
Signal* pitches[NN];

float randf() {
	return (float)rand() / (float)RAND_MAX;
}

int main(int argc, char* args[]) {
	SDL_AudioSpec wanted;

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		return(1);
	}

	/* Set the audio format */
	wanted.freq = SAMPLE_RATE;
	wanted.format = AUDIO_F32SYS;
	wanted.channels = 1;
	wanted.samples = SIGNAL_BUFFER_SIZE;
	wanted.callback = fill_audio;
	wanted.userdata = NULL;

	/* Open the audio device, forcing the desired format */
	if (SDL_OpenAudio(&wanted, NULL) < 0) {
		return(-1);
	}

	init_synths2();

	SDL_PauseAudio(0);

	SDL_Delay(1000 * 60 * 60);

	SDL_CloseAudio();

	free_synths();

	fclose(out);

	return(0);
}

int main2(int argc, char* args[]) {
	init_synths();
	out = fopen("outfile.bin", "wb");

	for (index_t i = 0; i < 100; i++) {
		SignalTree_Process(tree);
		fwrite(tree->signals[NN*2]->output, sizeof(float), SIGNAL_BUFFER_SIZE, out);
	}

	fclose(out);

	return(0);
}

void fill_audio(void *udata, Uint8 *stream, int len) {
	assert(len == SIGNAL_BUFFER_SIZE * sizeof(float));
	SignalTree_Process(tree);
	memcpy(stream, tree->signals[NN*2]->output, SIGNAL_BUFFER_SIZE * sizeof(float));
	index_t i = randf() * NN;
	if (i == NN) i--;
	Signal_SetConstant(pitches[i], powf(randf(), 2)*1000.0f + 20.f);
}

void init_synths() {
	srand(22222);

	index_t n_table = 2048;
	index_t n_waves = 1;
	index_t relative_freqs[] = { 1 };
	float relative_amps[] = { 1.0f };

	tree = SignalTree_Init(N_SIGNALS);

	float root_period = (randf()*3.0f + 5.0f) * 60.0f;
	Signal *root_constant = Signal_InitConstant(1.0f / root_period);
	Signal *root = Signal_InitSineTable(n_table, n_waves, relative_freqs, relative_amps, 10.0f, 40.0f, 0.0f);

	float sigma_period = (randf()*3.0f + 5.0f) * 60.0f;
	Signal *sigma_constant = Signal_InitConstant(1.0f / sigma_period);
	Signal *sigma = Signal_InitSineTable(n_table, n_waves, relative_freqs, relative_amps, 1.0f, 0.0f, 0.0f);

	Signal *mixer = Signal_InitMixer(N_SYNTHS_PER_NODE * N_NODES);

	SignalTree_AddSignal(tree, INDEX_ROOT - 1, root_constant);
	SignalTree_AddSignal(tree, INDEX_ROOT, root);
	SignalTree_AddSignal(tree, INDEX_SIGMA - 1, sigma_constant);
	SignalTree_AddSignal(tree, INDEX_SIGMA, sigma);
	SignalTree_AddSignal(tree, INDEX_MIXER, mixer);

	SignalTree_Connect(tree, INDEX_ROOT - 1, INDEX_ROOT, 0);
	SignalTree_Connect(tree, INDEX_SIGMA - 1, INDEX_SIGMA, 0);

	for (int n = 0; n < N_NODES; n++) {
		for (int i = 0; i < N_SYNTHS_PER_NODE; i++) {
			index_t index_synth = NODES_START + n*N_SYNTHS_PER_NODE * 2 + i * 2 + 1;
			float *weights = malloc(sizeof(float) * 2);
			weights[0] = (float)NODES_START + (float)n;
			weights[1] = randf()*100.0f - 50.f;
			Signal *pitch = Signal_InitLinearCombination(2, weights, true);
			Signal *synth = Signal_InitSineTable(n_table, n_waves, relative_freqs, relative_amps, 0.005f, 0.0f, randf());
			SignalTree_AddSignal(tree, index_synth - 1, pitch);
			SignalTree_AddSignal(tree, index_synth, synth);
			SignalTree_Connect(tree, INDEX_ROOT, index_synth - 1, 0);
			SignalTree_Connect(tree, INDEX_SIGMA, index_synth - 1, 1);
			SignalTree_Connect(tree, index_synth - 1, index_synth, 0);
			SignalTree_Connect(tree, index_synth, INDEX_MIXER, n*N_SYNTHS_PER_NODE + i);
		}
	}
}

void init_synths2() {
	srand(time(NULL));
	index_t n_table = 2048;
	index_t n_waves = 1;
	index_t relative_freqs[] = { 1 };
	tree = SignalTree_Init(NN * 2 + 1);
	Signal *mixer = Signal_InitMixer(NN);
	SignalTree_AddSignal(tree, NN*2, mixer);
	float relative_amps[] = { 1.0f };
	for (int n = 0; n < NN; n++) {
		float pitch_constant = powf(randf(), 2)*1000.0f + 20.f;
		Signal *pitch = Signal_InitConstant(pitch_constant);
		pitches[n] = pitch;
		Signal *synth = Signal_InitSineTable(n_table, n_waves, relative_freqs, relative_amps, 0.01f, 0.0f, randf());
		SignalTree_AddSignal(tree, n * 2, pitch);
		SignalTree_AddSignal(tree, n * 2 + 1, synth);
		SignalTree_Connect(tree, n * 2, n * 2 + 1, 0);
		SignalTree_Connect(tree, n * 2 + 1, NN * 2, n);
	}
}

void free_synths() {
	SignalTree_FreeAll(tree);
}