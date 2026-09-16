#pragma once

#include "ParameterIndex.h"
#include "SmoothValue.h"

#include <cstddef>

struct MixBusRoute
{
	static constexpr size_t kMaxChannels = 4;

	enum Format {
		kMono,
		kStereo,
		kQuad
	};

	Format format = kMono;
	size_t outputChannels[kMaxChannels] = {0, 1, 2, 3};

	static MixBusRoute mono(size_t outputChannel) {
		MixBusRoute route;
		route.format = kMono;
		route.outputChannels[0] = outputChannel;
		return route;
	}

	static MixBusRoute stereo(size_t ch0, size_t ch1) {
		MixBusRoute route;
		route.format = kStereo;
		route.outputChannels[0] = ch0;
		route.outputChannels[1] = ch1;
		return route;
	}

	static MixBusRoute quad(size_t ch0, size_t ch1, size_t ch2, size_t ch3) {
		MixBusRoute route;
		route.format = kQuad;
		route.outputChannels[0] = ch0;
		route.outputChannels[1] = ch1;
		route.outputChannels[2] = ch2;
		route.outputChannels[3] = ch3;
		return route;
	}
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
	static constexpr size_t kMaxChannels = MixBusRoute::kMaxChannels;

	void applyGain();
	void mixToMaster(float* master, size_t masterChannelCount);

	virtual void processEffects(float lfoBuf);

	size_t channelCount = 2;
	size_t outputChannels[kMaxChannels] = {0, 1, 2, 3};

	float sum[kMaxChannels] = {0.f, 0.f, 0.f, 0.f};

	SmoothValue mute = 1.f;
	SmoothValue volume = 1.f;
};
