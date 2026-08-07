#pragma once

#include "MixBusBase.h"
#include "MixBusFilterSection.h"
#include "BitCrusher.h"
#include "ParameterValue.h"

class FilterMixBus : public MixBusBase
{
public:
	void init(double sampleRate, const MixBusRoute& route) override;
	void setParameterValue(ParameterIndex index, float value) override;

protected:
	void processEffects() override;

	MixBusFilterSection lowpassSection;
	MixBusFilterSection highpassSection;

	ParameterValue bitCrushRate;
	BitCrusher bitCrush;
};
