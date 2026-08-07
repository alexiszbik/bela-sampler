#include "HatMixBus.h"

void HatMixBus::init(double sampleRate, const MixBusRoute& route) {
	FilterMixBus::init(sampleRate, route);

	reverbEffect.init(static_cast<int>(channelCount), sampleRate);

	flangerSpeed.setValue(0.f);
	flangerLevel.setValue(0.f);
	flanger.init(static_cast<int>(channelCount), sampleRate);
	flanger.setDepth(0.5f);
	flanger.setFeedback(0.85f);
}

void HatMixBus::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case ReverbSend:
			reverbEffect.setSendLevel(value);
			return;

		case FlangerSpeed:
			flangerSpeed.setValue(value * value * value);
			return;

		case FlangerLevel:
			flangerLevel.setValue(value);
			return;

		default:
			break;
	}

	FilterMixBus::setParameterValue(index, value);
}

void HatMixBus::processEffects() {
	FilterMixBus::processEffects();

	if(flangerSpeed.valueHasChanged) {
		const float speed = flangerSpeed.getValue();
		flanger.setRate(0.05f + speed * 4.95f);
	}

	if(flangerLevel.valueHasChanged) {
		const float level = flangerLevel.getValue();
		flanger.setMix(level * level);
	}

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = flanger.process(sum[channel], channel);
	}

	reverbEffect.process(sum, channelCount);
}
