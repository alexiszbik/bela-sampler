#pragma once

#include "Buffer.h"
#include "DelayLine.h"
#include "ParameterIndex.h"
#include "SmoothValue.h"
#include "BiquadFilter.h"

#include <cstddef>
#include <array>

class DelayMixBusSection
{
public:
	void init(size_t channelCount, double sampleRate);
	bool setParameterValue(ParameterIndex index, float value);
	void setTempo(double tempo);
	void process(float* sum, size_t channelCount);

private:
	void setDelayRate(float value);

private:
	static constexpr size_t kMaxChannels = 4;
	static constexpr float kMaxDelayMs = 500.f;

	DelayLine delayLine {kMaxDelayMs};
	BiquadFilter fbkFilter[kMaxChannels];
	SmoothValue delayTime = SmoothValue(250, 10000);
	SmoothValue delayLevel = 0;
	Buffer workBuf = 0;

	float feedback = 0.5f;
	double currentTempo = 120.0;
	bool isDelaySync = false;
	float freeDelayMs = 250.f;
	float currentRate = 0.25f;
};
