#pragma once

#include "BiquadFilter.h"
#include "ParameterValue.h"

#include <cstddef>

enum class MixBusFilterType {
	Lowpass,
	Highpass
};

class MixBusFilterSection
{
public:
	static constexpr size_t kMaxChannels = 2;

	void init(float sampleRate, MixBusFilterType type, size_t channelCount);

	void setCutoffRatio(float ratio);
	void applyPending();

	float process(size_t channel, float input);
	void reset();

private:
	void applyCutoffHz(float cutoffHz);

	BiquadFilter filters[kMaxChannels];
	ParameterValue cutoffHz;
	MixBusFilterType filterType = MixBusFilterType::Lowpass;
	float sampleRate = 44100.f;
	size_t channelCount;
	float maxCutoffHz; 
};
