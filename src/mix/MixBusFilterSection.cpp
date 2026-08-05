#include "MixBusFilterSection.h"

#include "CutoffHelper.h"

static constexpr float kFilterQ = 1.5f;
static constexpr float kNyquistMargin = 0.49f;

void MixBusFilterSection::init(float inSampleRate, MixBusFilterType type, size_t inChannelCount) {
	sampleRate = inSampleRate;
	filterType = type;
	channelCount = inChannelCount;

	maxCutoffHz = sampleRate * kNyquistMargin;

	for(size_t channel = 0; channel < kMaxChannels; channel++) {
		filters[channel].init(sampleRate);
	}

	reset();
}

void MixBusFilterSection::setCutoffRatio(float ratio) {
	cutoffHz.setValue(cutoffRatioToHz(ratio));
}

void MixBusFilterSection::applyCutoffHz(float cutoffHzValue) {

	float clampedHz = cutoffHzValue;

	if(clampedHz < kMinCutoffHz) {
		clampedHz = kMinCutoffHz;
	} else if(clampedHz > maxCutoffHz) {
		clampedHz = maxCutoffHz;
	}

	for(size_t channel = 0; channel < channelCount && channel < kMaxChannels; channel++) {
		if(filterType == MixBusFilterType::Lowpass) {
			filters[channel].setLowpass(clampedHz, kFilterQ);
		} else {
			filters[channel].setHighpass(clampedHz, kFilterQ);
		}
	}
}

void MixBusFilterSection::applyPending() {
	if(!cutoffHz.valueHasChanged) {
		return;
	}

	const float pendingCutoffHz = cutoffHz.getValue();
	applyCutoffHz(pendingCutoffHz);
}

float MixBusFilterSection::process(size_t channel, float input) {
	if(channel >= kMaxChannels) {
		return input;
	}

	return filters[channel].process(input);
}

void MixBusFilterSection::reset() {
	for(size_t channel = 0; channel < kMaxChannels; channel++) {
		filters[channel].reset();
	}
}
