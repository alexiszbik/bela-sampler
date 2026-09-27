#include "FXMixBus.h"

void FXMixBus::init(double sampleRate, const MixBusRoute& route) {
	FilterMixBus::init(sampleRate, route);

	delaySection.init(channelCount, sampleRate);

	flangerSpeed.setValue(0.f);
	flangerLevel.setValue(0.f);
	flanger.init(static_cast<int>(channelCount), sampleRate);
	flanger.setDepth(0.5f);
	flanger.setFeedback(0.6f);
}

void FXMixBus::setParameterValue(ParameterIndex index, float value) {
	if(delaySection.setParameterValue(index, value)) {
		return;
	}

	switch(index) {
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

void FXMixBus::setTempo(double tempo) {
	FilterMixBus::setTempo(tempo);
	delaySection.setTempo(tempo);
}

void FXMixBus::processEffects(const TriLfo& lfo) {
	FilterMixBus::processEffects(lfo);

	if(flangerSpeed.valueHasChanged) {
		const float speed = flangerSpeed.getValue();
		flanger.setRate(0.05f + speed * 4.95f);
	}

	if(flangerLevel.valueHasChanged) {
		const float level = flangerLevel.getValue();
		flanger.setMix(level * level);
	}

	for(size_t channel = 0; channel < channelCount; ++channel) {
		sum[channel] = flanger.process(sum[channel], static_cast<int>(channel));
	}

	delaySection.process(sum, channelCount);
}
