#pragma once

#include "FilterMixBus.h"
#include "ReverbEffect.h"

class SnareMixBus : public FilterMixBus
{
public:
	void init(double sampleRate, const MixBusRoute& route) override;
	void setParameterValue(ParameterIndex index, float value) override;

protected:
	void processEffects() override;

private:
	ReverbEffect reverbEffect;
};
