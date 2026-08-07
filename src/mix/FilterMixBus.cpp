#include "FilterMixBus.h"

void FilterMixBus::init(double sampleRate, const MixBusRoute& route) {
	MixBusBase::init(sampleRate, route);

	const float fsr = static_cast<float>(sampleRate);

	lowpassSection.init(fsr, MixBusFilterType::Lowpass, channelCount);
	highpassSection.init(fsr, MixBusFilterType::Highpass, channelCount);

	lowpassSection.setCutoffRatio(1.0f);
	highpassSection.setCutoffRatio(0.0f);

	lowpassSection.applyPending();
	highpassSection.applyPending();

	lowpassSection.reset();
	highpassSection.reset();

	bitCrushRate.setValue(1.f);
	bitCrush.init(channelCount);
}

void FilterMixBus::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case LowPassCutoff:
			lowpassSection.setCutoffRatio(value);
			return;

		case HiPassCutoff:
			highpassSection.setCutoffRatio(value);
			return;

		case BitCrushRate:
			bitCrushRate.setValue(value);
			return;

		default:
			break;
	}

	MixBusBase::setParameterValue(index, value);
}

void FilterMixBus::processEffects() {
	if(bitCrushRate.valueHasChanged) {
		bitCrush.setRepeatRate(bitCrushRate.getValue());
	}

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = bitCrush.process(sum[channel], channel);
	}

	lowpassSection.applyPending();
	highpassSection.applyPending();

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = lowpassSection.process(channel, sum[channel]);
		sum[channel] = highpassSection.process(channel, sum[channel]);
	}
}
