#pragma once

#include "Buffer.h"
#include "Reverb.h"
#include "SmoothValue.h"
#include "TriLfo.h"

#include <cstddef>

class ReverbEffect
{
public:
	void init(int channelCount, double sampleRate);
	void setSendLevel(float value);
	void process(float* sum, size_t channelCount, bool enabled = true);

private:
	Reverb reverb;
	TriLfo lfo;
	SmoothValue sendLevel = 0;
	Buffer reverbTime = 8.f;
};
