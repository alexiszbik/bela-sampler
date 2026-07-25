#include "MixBus.h"

void MixBus::init(double sampleRate, float lowpassFreq, float q) {
	const float sampleRateF = static_cast<float>(sampleRate);
	for(size_t channel = 0; channel < kChannelCount; channel++) {
		filters[channel].init(sampleRateF);
		filters[channel].setLowpass(lowpassFreq, q);
		filters[channel].reset();
	}
}

void MixBus::process(float* buf, size_t channelCount) {
	const size_t channelsToProcess = channelCount < kChannelCount ? channelCount : kChannelCount;
	for(size_t channel = 0; channel < channelsToProcess; channel++) {
		buf[channel] = filters[channel].process(buf[channel]);
	}
}
