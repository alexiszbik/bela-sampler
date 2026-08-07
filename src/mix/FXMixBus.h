#pragma once

#include "MixBusBase.h"
#include "MixBusFilterSection.h"
#include "DelayLine.h"
#include "Reverb.h"
#include "TriLfo.h"
#include "BitCrusher.h"
#include "Flanger.h"

class FXMixBus : public MixBusBase
{
public:
	void init(double sampleRate, const MixBusRoute& route) override;
	void setParameterValue(ParameterIndex index, float value) override;

	bool enableReverb = false;

protected:
	void processEffects() override;

private:
	MixBusFilterSection lowpassSection;
	MixBusFilterSection highpassSection;

	DelayLine delayLine{500.f};
	Reverb reverb;

	SmoothValue delayTime = 250;
	SmoothValue delayLevel = 0;

	SmoothValue reverbSendLevel = 0;

	float feedback = 0.5f;
	Buffer workBuf = 0;
	Buffer reverbTime = 8.f;

	ParameterValue bitCrushRate;
	ParameterValue flangerSpeed;
	ParameterValue flangerLevel;

	TriLfo rvbLfo;

	BitCrusher bitCrush;
	Flanger flanger;
};
