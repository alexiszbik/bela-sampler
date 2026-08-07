#pragma once

#include "FilterMixBus.h"
#include "ReverbEffect.h"
#include "Flanger.h"
#include "ParameterValue.h"

class HatMixBus : public FilterMixBus
{
public:
	void init(double sampleRate, const MixBusRoute& route) override;
	void setParameterValue(ParameterIndex index, float value) override;

protected:
	void processEffects() override;

private:
	ReverbEffect reverbEffect;
	Flanger flanger;

	ParameterValue flangerSpeed;
	ParameterValue flangerLevel;
};
