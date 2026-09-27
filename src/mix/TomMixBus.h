#pragma once

#include "DelayMixBusSection.h"
#include "FilterMixBus.h"

class TomMixBus : public FilterMixBus
{
public:
	void init(double sampleRate, const MixBusRoute& route) override;
	void setParameterValue(ParameterIndex index, float value) override;
	void setTempo(double tempo) override;

protected:
	void processEffects(const TriLfo& lfo) override;

private:
	DelayMixBusSection delaySection;
};
