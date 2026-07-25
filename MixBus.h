#pragma once

#include "BiquadFilter.h"

#include <cstddef>

class MixBus
{
public:
	void init(double sampleRate, float lowpassFreq, float q = 1.f);
	void setLowpassCutoff(float cutoffFreq);
	void process(float* buf, size_t channelCount);

private:
	static constexpr size_t kChannelCount = 2;
	BiquadFilter filters[kChannelCount];
	float lowpassQ = 1.f;
};
