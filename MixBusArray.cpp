#include "MixBusArray.h"

static constexpr float kMixBus0LowpassFreq = 800.f;
static constexpr float kMixBus1LowpassFreq = 4500.f;
static constexpr float kMixBusLowpassQ = 1.f;

void MixBusArray::init(double sampleRate) {
	buses[0].init(sampleRate, kMixBus0LowpassFreq, kMixBusLowpassQ);
	buses[1].init(sampleRate, kMixBus1LowpassFreq, kMixBusLowpassQ);
}

void MixBusArray::processBuses(float busSums[kBusCount][kBusChannels], size_t channelCount) {
	for(size_t bus = 0; bus < kBusCount; bus++) {
		buses[bus].process(busSums[bus], channelCount);
	}
}

void MixBusArray::writeToOutput(const float busSums[kBusCount][kBusChannels], float* out, size_t outChannelCount) const {
	if(outChannelCount >= 1) {
		out[0] = busSums[0][0];
	}
	if(outChannelCount >= 2) {
		out[1] = busSums[0][1];
	}
	if(outChannelCount >= 3) {
		out[2] = busSums[1][0];
	}
	if(outChannelCount >= 4) {
		out[3] = busSums[1][1];
	}
}
