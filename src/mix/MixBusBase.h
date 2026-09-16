#pragma once

#include "ParameterIndex.h"
#include "SmoothValue.h"

#include <cstddef>

struct MixBusRoute
{
	enum Format {
		kMono,
		kStereo,
		kQuad
	};

	Format format = kMono;
	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
	size_t outputChannel2 = 2;
	size_t outputChannel3 = 3;

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
	void processAndMixTo(float* master, size_t masterChannelCount, float lfoValue);

protected:
	static constexpr size_t kMaxChannels = 4;

	void applyGain();
	void mixToMaster(float* master, size_t masterChannelCount);

	virtual void processEffects(float lfoBuf);

	size_t channelCount = 2;

	size_t outputChannel0 = 0;
	size_t outputChannel1 = 1;
	size_t outputChannel2 = 2;
	size_t outputChannel3 = 3;

	float sum[kMaxChannels] = {0.f, 0.f, 0.f, 0.f};

	SmoothValue mute = 1.f;
	SmoothValue volume = 1.f;
};
