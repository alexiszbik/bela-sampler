/*
 * Sampler — Bela Gem C++ project
 * https://bela.io
 */

#include <Bela.h>

#include "MixBusArray.h"
#include "MidiInput.h"
#include "ProgramBank.h"
#include "SamplerBootstrap.h"
#include "SamplerEngine.h"
#include "Sample.h"

#include <vector>

static const size_t kNumPlayers = 32;

static std::vector<Sample> gSamples;
static ProgramBank gProgramBank;
static SamplerEngine gEngine;
static MidiInput gMidiInput;

float mix[MixBusArray::kMasterChannelCount] = {};

const int kOutChannelStart = 2;

bool setup(BelaContext* context, void* userData) {
	(void)userData;

	if(!SamplerBootstrap::init(gSamples, gProgramBank, gEngine, context->audioSampleRate, kNumPlayers)) {
		return false;
	}

	gMidiInput.setup(&gEngine);
	return true;
}

void render(BelaContext* context, void* userData) {
	(void)userData;

	for(unsigned int n = 0; n < context->audioFrames; n++) {
		for(unsigned int channel = 0; channel < MixBusArray::kMasterChannelCount; channel++) {
			mix[channel] = 0;
		}

		gEngine.nextSamples(mix, MixBusArray::kMasterChannelCount);

		for(unsigned int channel = 0; channel < MixBusArray::kMasterChannelCount; channel++) {
			audioWrite(context, n, channel + kOutChannelStart, mix[channel]);
		}
	}
}

void cleanup(BelaContext* context, void* userData) {
	(void)context;
	(void)userData;
	gSamples.clear();
}
