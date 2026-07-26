#include "MixBus.h"

static constexpr float kMixBusLowpassQ = 1.f;
static constexpr float kMixBusDefaultLowpassHz = 4500.f;

void MixBus::init(double sampleRate, const MixBusRoute& route) {
	channelCount = route.mono ? 1u : 2u;
	outputChannel0 = route.outputChannel0;
	outputChannel1 = route.outputChannel1;

	const float sampleRateF = static_cast<float>(sampleRate);
	for(size_t channel = 0; channel < kMaxChannels; channel++) {
		filters[channel].init(sampleRateF);
		filters[channel].reset();
	}

	setLowpassCutoff(kMixBusDefaultLowpassHz);
	clearSum();
}

void MixBus::clearSum() {
	sum[0] = 0.f;
	sum[1] = 0.f;
}

float* MixBus::getSum() {
	return sum;
}

void MixBus::setLowpassCutoff(float cutoffFreq) {
	for(size_t channel = 0; channel < channelCount; channel++) {
		filters[channel].setLowpass(cutoffFreq, kMixBusLowpassQ);
	}
}

void MixBus::processAndMixTo(float* master, size_t masterChannelCount) {
	if(master == nullptr || masterChannelCount == 0) {
		return;
	}

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = filters[channel].process(sum[channel]);
	}

	if(outputChannel0 < masterChannelCount) {
		master[outputChannel0] += sum[0];
	}

	if(channelCount > 1 && outputChannel1 < masterChannelCount) {
		master[outputChannel1] += sum[1];
	}
}
