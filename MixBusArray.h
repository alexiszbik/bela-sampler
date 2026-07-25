#pragma once

#include "MixBus.h"

#include <cstddef>

class MixBusArray
{
public:
	static constexpr size_t kBusCount = 2;
	static constexpr size_t kBusChannels = 2;
	static constexpr size_t kOutputChannels = kBusCount * kBusChannels;

	void init(double sampleRate);
	void processBuses(float busSums[kBusCount][kBusChannels], size_t channelCount);
	void writeToOutput(const float busSums[kBusCount][kBusChannels], float* out, size_t outChannelCount) const;

private:
	MixBus buses[kBusCount];
};
