#pragma once

#include "MixBusFilterSection.h"
#include "ParameterIndex.h"
#include "SmoothValue.h"
#include "DelayLine.h"

#include <cstddef>

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
	static constexpr size_t kMaxChannels = 2;

	size_t channelCount = 2;
	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
	float sum[kMaxChannels] = {0.f, 0.f};

	MixBusFilterSection lowpassSection;
	MixBusFilterSection highpassSection;
	SmoothValue mute;

	DelayLine delayLine{500.f};

	Buffer delayTime = 250;
	Buffer feedback = 0.5;
	Buffer workBuf = 0;

	float volume = 1.f;
};
