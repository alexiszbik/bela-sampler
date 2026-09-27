#include "TomMixBus.h"

void TomMixBus::init(double sampleRate, const MixBusRoute& route) {
	FilterMixBus::init(sampleRate, route);
	delaySection.init(channelCount, sampleRate);
}

void TomMixBus::setParameterValue(ParameterIndex index, float value) {
	if(delaySection.setParameterValue(index, value)) {
		return;
	}

	FilterMixBus::setParameterValue(index, value);
}

void TomMixBus::setTempo(double tempo) {
	FilterMixBus::setTempo(tempo);
	delaySection.setTempo(tempo);
}

void TomMixBus::processEffects(const TriLfo& lfo) {
	FilterMixBus::processEffects(lfo);
	delaySection.process(sum, channelCount);
}
