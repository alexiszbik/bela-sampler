#pragma once

#include "BiquadFilter.h"

#include <cstddef>

enum ParameterIndex {
	Volume, 
	Mute,
	LowPassCutoff,
	HiPassCutoff
};

struct ParameterValue {
	float value = 0.f;
	bool valueHasChanged = true;
	
	void setValue(float newValue) {
		if (value != newValue) {
			value = newValue;
			valueHasChanged = true;
		}
	}

	float getValue() {
		valueHasChanged = false;
		return value;
	}
};

struct MixBusRoute
{
	bool mono = false;
	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
};

class MixBus
{
public:
	void init(double sampleRate, const MixBusRoute& route);
	void clearSum();
	float* getSum();

	size_t getChannelCount() const { return channelCount; }
	bool isMono() const { return channelCount <= 1; }

	void setParameterValue(ParameterIndex index, float value);

	void processAndMixTo(float* master, size_t masterChannelCount);

private:
	void setLowpassCutoff(float cutoffRatio);
	void setHipassCutoff(float cutoffRatio);

private:
	static constexpr size_t kMaxChannels = 2;

	size_t channelCount = 2;
	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
	float sum[kMaxChannels] = {0.f, 0.f};

	ParameterValue lpFreq;
	ParameterValue hpFreq;

	BiquadFilter lpFilters[kMaxChannels];
	BiquadFilter hpFilters[kMaxChannels];
};
