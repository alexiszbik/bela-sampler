#pragma once

#include "FilterMixBus.h"
#include "DelayLine.h"
#include "ReverbEffect.h"
#include "Flanger.h"
#include "ParameterValue.h"

class FXMixBus : public FilterMixBus
{
public:
	void init(double sampleRate, const MixBusRoute& route) override;
	void setParameterValue(ParameterIndex index, float value) override;

protected:
	void processEffects() override;

private:
	DelayLine delayLine{500.f};
	Flanger flanger;

	SmoothValue delayTime = 250;
	SmoothValue delayLevel = 0;

	float feedback = 0.5f;
	Buffer workBuf = 0;

	ParameterValue flangerSpeed;
	ParameterValue flangerLevel;
};
