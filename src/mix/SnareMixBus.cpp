#include "SnareMixBus.h"

void SnareMixBus::init(double sampleRate, const MixBusRoute& route) {
	FilterMixBus::init(sampleRate, route);
	reverbEffect.init(static_cast<int>(channelCount), sampleRate);
}

void SnareMixBus::setParameterValue(ParameterIndex index, float value) {
	if(index == ReverbSend) {
		reverbEffect.setSendLevel(value);
		return;
	}

	FilterMixBus::setParameterValue(index, value);
}

void SnareMixBus::processEffects(float lfoBuf) {
	FilterMixBus::processEffects(lfoBuf);
	reverbEffect.process(sum, channelCount);
}
