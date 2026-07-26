#include "MixBus.h"

static constexpr float kMixBusLowpassQ = 1.f;

void MixBus::init(double sampleRate) {
	const float fsr = static_cast<float>(sampleRate);
	for(size_t channel = 0; channel < kChannelCount; channel++) {
		filters[channel].init(fsr);
	}

	setLowpassCutoff(4500);

	for(size_t channel = 0; channel < kChannelCount; channel++) {
		filters[channel].reset();
	}
}

void MixBus::setLowpassCutoff(float cutoffFreq) {
	for(size_t channel = 0; channel < kChannelCount; channel++) {
		filters[channel].setLowpass(cutoffFreq, kMixBusLowpassQ);
	}
}

void MixBus::process(float* buf, size_t channelCount) {
	const size_t channelsToProcess = channelCount < kChannelCount ? channelCount : kChannelCount;
	for(size_t channel = 0; channel < channelsToProcess; channel++) {
		buf[channel] = filters[channel].process(buf[channel]);
	}
}
