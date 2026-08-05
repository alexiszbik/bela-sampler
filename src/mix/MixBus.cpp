#include "MixBus.h"

#include "CutoffHelper.h"

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
	lpFreq.setValue(cutoffRatioToHz(cutoffRatio));
}

void MixBus::setHipassCutoff(float cutoffRatio) {
	hpFreq.setValue(cutoffRatioToHz(cutoffRatio));
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

	if (lpFreq.valueHasChanged) {
		float f = lpFreq.getValue();
		for(size_t channel = 0; channel < channelCount; channel++) {
			lpFilters[channel].setLowpass(f, kMixBusLowpassQ);
		}
	}

	if (hpFreq.valueHasChanged) {
		float f = hpFreq.getValue();
		for(size_t channel = 0; channel < channelCount; channel++) {
			hpFilters[channel].setHighpass(f, kMixBusLowpassQ);
		}
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
