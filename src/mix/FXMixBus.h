#pragma once

#include "DelayMixBusSection.h"
#include "FilterMixBus.h"
#include "Flanger.h"
#include "ParameterValue.h"

class FXMixBus : public FilterMixBus
{
public:
	void init(double sampleRate, const MixBusRoute& route) override;
	void setParameterValue(ParameterIndex index, float value) override;
	void setTempo(double tempo) override;

protected:
	void processEffects(const TriLfo& lfo) override;

private:
	DelayMixBusSection delaySection;
	Flanger flanger;

	ParameterValue flangerSpeed;
	ParameterValue flangerLevel;
};
