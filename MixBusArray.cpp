#include "MixBusArray.h"

#include <cmath>

static constexpr float kMixBus0LowpassFreq = 800.f;
static constexpr float kMixBus1LowpassFreq = 4500.f;
static constexpr float kMixBusLowpassQ = 1.f;
static constexpr float kMixBus1MinCutoffHz = 80.f;
static constexpr float kMixBus1MaxCutoffHz = 16000.f;

float MixBusArray::bus1CutoffFromCc(int ccValue) {
	if(ccValue <= 0) {
		return kMixBus1MinCutoffHz;
	}

	if(ccValue >= 127) {
		return kMixBus1MaxCutoffHz;
	}

	const float normalized = static_cast<float>(ccValue) / 127.f;
	const float ratio = kMixBus1MaxCutoffHz / kMixBus1MinCutoffHz;
	return kMixBus1MinCutoffHz * static_cast<float>(std::pow(static_cast<double>(ratio), static_cast<double>(normalized)));
}

void MixBusArray::init(double sampleRate) {
	buses[0].init(sampleRate, kMixBus0LowpassFreq, kMixBusLowpassQ);
	buses[1].init(sampleRate, kMixBus1LowpassFreq, kMixBusLowpassQ);
}

void MixBusArray::setBusLowpassCutoff(size_t busIndex, float cutoffFreq) {
	if(busIndex >= kBusCount) {
		return;
	}

	buses[busIndex].setLowpassCutoff(cutoffFreq);
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
