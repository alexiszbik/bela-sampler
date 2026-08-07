#pragma once

#include "ParameterIndex.h"
#include "SmoothValue.h"

#include <cstddef>

struct MixBusRoute
{
	bool mono = false;
	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
};

class MixBusBase
{
public:
	virtual ~MixBusBase() = default;

	virtual void init(double sampleRate, const MixBusRoute& route);
	void clearSum();
	float* getSum();

	size_t getChannelCount() const { return channelCount; }
	bool isMono() const { return channelCount <= 1; }

	virtual void setParameterValue(ParameterIndex index, float value);
	virtual void processAndMixTo(float* master, size_t masterChannelCount);

protected:
	static constexpr size_t kMaxChannels = 2;

	void applyGain();
	void mixToMaster(float* master, size_t masterChannelCount);

	virtual void processEffects();

	size_t channelCount = 2;
	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
	float sum[kMaxChannels] = {0.f, 0.f};

	SmoothValue mute = 1.f;
	SmoothValue volume = 1.f;
};
