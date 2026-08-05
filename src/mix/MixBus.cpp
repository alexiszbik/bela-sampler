#include "MixBus.h"

#include <cmath>

static constexpr float kMinCutoffHz = 20.f;
static constexpr float kMaxCutoffHz = 20000.f;

static constexpr float kMixBusLowpassQ = 1.f;

void MixBus::init(double sampleRate, const MixBusRoute& route) {
	channelCount = route.mono ? 1u : 2u;
	outputChannel0 = route.outputChannel0;
	outputChannel1 = route.outputChannel1;

	const float fsr = static_cast<float>(sampleRate);

	for(size_t channel = 0; channel < kMaxChannels; channel++) {
		lpFilters[channel].init(fsr);
		hpFilters[channel].init(fsr);
	}

	setLowpassCutoff(1.0f);
	setHipassCutoff(0.0f);

	for(size_t channel = 0; channel < kMaxChannels; channel++) {
		lpFilters[channel].reset();
		hpFilters[channel].reset();
	}

	clearSum();
}

void MixBus::clearSum() {
	sum[0] = 0.f;
	sum[1] = 0.f;
}

float* MixBus::getSum() {
	return sum;
}

void MixBus::setLowpassCutoff(float cutoffRatio) {
	const float ratio = kMaxCutoffHz / kMinCutoffHz;
	float cutoffValue = kMinCutoffHz * static_cast<float>(std::pow(static_cast<double>(ratio), static_cast<double>(cutoffRatio)));

	if (lpFreq != cutoffValue) {
		lpFreq = cutoffValue;
		lpFreqHasChanged = true;
	}
}

void MixBus::setHipassCutoff(float cutoffRatio) {
	const float ratio = kMaxCutoffHz / kMinCutoffHz;
	float cutoffValue = kMinCutoffHz * static_cast<float>(std::pow(static_cast<double>(ratio), static_cast<double>(cutoffRatio)));

	if (hpFreq != cutoffValue) {
		hpFreq = cutoffValue;
		hpFreqHasChanged = true;
	}
}

void MixBus::setParameterValue(ParameterIndex index, float value) {
	switch (index) {
		case Volume : {

		} break;

		case Mute : {

		} break;

		case LowPassCutoff : {
			setLowpassCutoff(value);
		} break;

		case HiPassCutoff : {
			setHipassCutoff(value);
		} break;
	}
}

void MixBus::processAndMixTo(float* master, size_t masterChannelCount) {
	if(master == nullptr || masterChannelCount == 0) {
		return;
	}

	if (lpFreqHasChanged) {
		for(size_t channel = 0; channel < channelCount; channel++) {
			lpFilters[channel].setLowpass(lpFreq, kMixBusLowpassQ);
		}
		lpFreqHasChanged = false;
	}

	if (hpFreqHasChanged) {
		for(size_t channel = 0; channel < channelCount; channel++) {
			hpFilters[channel].setHighpass(hpFreq, kMixBusLowpassQ);
		}
		hpFreqHasChanged = false;
	}


	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = lpFilters[channel].process(sum[channel]);
		sum[channel] = hpFilters[channel].process(sum[channel]);
	}

	if(outputChannel0 < masterChannelCount) {
		master[outputChannel0] += sum[0];
	}

	if(channelCount > 1 && outputChannel1 < masterChannelCount) {
		master[outputChannel1] += sum[1];
	}
}
