#define _CRT_SECURE_NO_DEPRECATE

#include <SignalTree.h>
#include <SignalConstant.h>
#include <SignalMixer.h>
#include <SignalPhaseTable.h>
#include <SignalAttenuator.h>
#include <SignalPinkNoise.h>
#include <SignalMultiPhaseTable.h>
#include <SDL.h>
#include <memory.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <crtdbg.h>

SignalTree *tree;
FILE *out;

void fill_audio(void *udata, Uint8 *stream, int len);
void init_synths();
void init_synths2();
void init_synths3();
void init_synths4();
void free_synths();

int main(int argc, char* args[])
{
	SDL_AudioSpec wanted;

	out = fopen("outfile.bin", "wb");

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

	init_synths3();

	SDL_PauseAudio(0);

	SDL_Delay(1000 * 60 * 40);

	SDL_CloseAudio();

	free_synths();

	fclose(out);

	return(0);
}

#define N_UNITS 1000
#define N_SYNTHS_PER_UNIT 2
#define MIXER_INDEX N_UNITS*N_SYNTHS_PER_UNIT


int main2(int argc, char* args[]) {
	init_synths3();
	out = fopen("outfile.bin", "wb");

	for (index_t i = 0; i < 100; i++) {
		SignalTree_Process(tree);
		fwrite(tree->signals[MIXER_INDEX]->output, sizeof(float), SIGNAL_BUFFER_SIZE, out);
	}

	fclose(out);

	return(0);
}

float r2()
{
	return (float)rand() / (float)RAND_MAX;
}

int test = 0;

void fill_audio(void *udata, Uint8 *stream, int len) {
	assert(len == SIGNAL_BUFFER_SIZE * sizeof(float));
	SignalTree_Process(tree);
	memcpy(stream, tree->signals[MIXER_INDEX]->output, SIGNAL_BUFFER_SIZE * sizeof(float));
	//fwrite(stream, sizeof(float), SIGNAL_BUFFER_SIZE, out);
}

void init_synths3() {
	srand(22222);
	tree = SignalTree_Init(MIXER_INDEX + 1);
	Signal *mixer = Signal_InitMixer(N_UNITS);
	SignalTree_AddSignal(tree, MIXER_INDEX, mixer);

	for (index_t i = 0; i < N_UNITS; i++) {
		index_t n_waves = 1;
		index_t relative_freqs[] = { 1, 2, 3, 4, 5 };
		float relative_amps[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
		Signal *frequency = Signal_InitConstant(powf(r2(), 1)*1000.0f + 10.0f);
		Signal *timbre = Signal_InitSineTable(2048, n_waves, relative_freqs, relative_amps, 0.8f / N_UNITS, 0.0f);
		SignalTree_AddSignal(tree, i * 2, frequency);
		SignalTree_AddSignal(tree, i * 2 + 1, timbre);
		SignalTree_Connect(tree, i * 2, i * 2 + 1, 0);
		SignalTree_Connect(tree, i * 2 + 1, MIXER_INDEX, i);
	}
}

void init_synths4() {
	srand(22222);
	tree = SignalTree_Init(1);
	float *freqs = malloc(sizeof(float) * N_UNITS);
	for (index_t i = 0; i < N_UNITS; i++) {
		freqs[i] = powf(r2(), 4)*0.0f + 220.0f;
	}
	index_t n_waves = 1;
	index_t relative_freqs[] = { 1, 2, 3, 4, 5 };
	float relative_amps[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	Signal *noise = Signal_InitMultiSineTable(N_UNITS, freqs, true, 2048, n_waves, relative_freqs, relative_amps, 0.8f / N_UNITS, 0.0f);
	SignalTree_AddSignal(tree, 0, noise);
}

void init_synths2() {
	tree = SignalTree_Init(1);

	Signal *pink = Signal_InitPinkNoise();

	SignalTree_AddSignal(tree, 0, pink);
}


void init_synths() {
	tree = SignalTree_Init(19);

	Signal *pulse = Signal_InitConstant(1.0f / 7.0f);
	Signal *vibrato = Signal_InitConstant(6.f);

	index_t relative_freqs[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	float relative_amps[] = { 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f, 0.1f, 0.1f, 0.1f };
	Signal *pitch = Signal_InitSineTable(2048, 1, relative_freqs, relative_amps, 0.3f, 313.0f*powf(2.0, 0.0f / 12.0f) - 0.3f);
	Signal *timbre = Signal_InitSineTable(2048, 12, relative_freqs, relative_amps, 0.1f, 0.0f);

	index_t ratios[] = { 8, 15 };
	float levels[] = { 0.0f, 1.0f, 0.0f };
	Signal *envelope = Signal_InitGADSRTable(2, ratios, levels);

	Signal *attenuator = Signal_InitAttenuator();


	Signal *pulse2 = Signal_InitConstant(1.0f / 6.0f);
	Signal *vibrato2 = Signal_InitConstant(6.2f);

	index_t relative_freqs2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	float relative_amps2[] = { 0.9f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f, 0.1f, 0.1f };
	Signal *pitch2 = Signal_InitSineTable(2048, 1, relative_freqs, relative_amps, 0.2f, 313.0f*powf(2.0, 5.0f / 12.0f) - 0.2f);
	Signal *timbre2 = Signal_InitSineTable(2048, 12, relative_freqs, relative_amps, 0.08f, 0.0f);

	index_t ratios2[] = { 5, 15 };
	float levels2[] = { 0.0f, 1.0f, 0.0f };
	Signal *envelope2 = Signal_InitGADSRTable(2, ratios, levels);

	Signal *attenuator2 = Signal_InitAttenuator();


	Signal *pulse3 = Signal_InitConstant(1.0f / 5.0f);
	Signal *vibrato3 = Signal_InitConstant(6.3f);

	index_t relative_freqs3[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	float relative_amps3[] = { 0.9f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f };
	Signal *pitch3 = Signal_InitSineTable(2048, 1, relative_freqs, relative_amps, 0.2f, 313.0f*powf(2.0, 4.0f / 12.0f) - 0.2f);
	Signal *timbre3 = Signal_InitSineTable(2048, 12, relative_freqs, relative_amps, 0.05f, 0.0f);

	index_t ratios3[] = { 5, 15 };
	float levels3[] = { 0.0f, 1.0f, 0.0f };
	Signal *envelope3 = Signal_InitGADSRTable(2, ratios, levels);

	Signal *attenuator3 = Signal_InitAttenuator();


	Signal *mixer = Signal_InitMixer(3);

	SignalTree_AddSignal(tree, 0, pulse);
	SignalTree_AddSignal(tree, 1, vibrato);
	SignalTree_AddSignal(tree, 2, pitch);
	SignalTree_AddSignal(tree, 3, timbre);
	SignalTree_AddSignal(tree, 4, envelope);
	SignalTree_AddSignal(tree, 5, attenuator);
	SignalTree_AddSignal(tree, 6, pulse2);
	SignalTree_AddSignal(tree, 7, vibrato2);
	SignalTree_AddSignal(tree, 8, pitch2);
	SignalTree_AddSignal(tree, 9, timbre2);
	SignalTree_AddSignal(tree, 10, envelope2);
	SignalTree_AddSignal(tree, 11, attenuator2);
	SignalTree_AddSignal(tree, 12, pulse3);
	SignalTree_AddSignal(tree, 13, vibrato3);
	SignalTree_AddSignal(tree, 14, pitch3);
	SignalTree_AddSignal(tree, 15, timbre3);
	SignalTree_AddSignal(tree, 16, envelope3);
	SignalTree_AddSignal(tree, 17, attenuator3);
	SignalTree_AddSignal(tree, 18, mixer);
	SignalTree_Connect(tree, 0, 4, 0);
	SignalTree_Connect(tree, 1, 2, 0);
	SignalTree_Connect(tree, 2, 3, 0);
	SignalTree_Connect(tree, 3, 5, 0);
	SignalTree_Connect(tree, 4, 5, 1);
	SignalTree_Connect(tree, 6, 10, 0);
	SignalTree_Connect(tree, 7, 8, 0);
	SignalTree_Connect(tree, 8, 9, 0);
	SignalTree_Connect(tree, 9, 11, 0);
	SignalTree_Connect(tree, 10, 11, 1);
	SignalTree_Connect(tree, 12, 16, 0);
	SignalTree_Connect(tree, 13, 14, 0);
	SignalTree_Connect(tree, 14, 15, 0);
	SignalTree_Connect(tree, 15, 17, 0);
	SignalTree_Connect(tree, 16, 17, 1);
	SignalTree_Connect(tree, 5, 18, 0);
	SignalTree_Connect(tree, 11, 18, 1);
	SignalTree_Connect(tree, 17, 18, 2);
}


void free_synths() {
	SignalTree_FreeAll(tree);
}