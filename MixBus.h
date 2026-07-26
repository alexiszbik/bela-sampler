#pragma once

#include "BiquadFilter.h"

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
	void setLowpassCutoff(float cutoffFreq);
	void processAndMixTo(float* master, size_t masterChannelCount);

private:
	static constexpr size_t kMaxChannels = 2;

	size_t channelCount = 2;
	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
	float sum[kMaxChannels] = {0.f, 0.f};
	BiquadFilter filters[kMaxChannels];
};
