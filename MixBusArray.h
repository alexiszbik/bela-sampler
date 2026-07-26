#pragma once

#include "MixBus.h"

#include <cstddef>

class MixBusArray
{
public:
	static constexpr size_t kBusCount = 2;
	static constexpr size_t kMasterChannelCount = 4;

	static constexpr int kBus1FilterCc = 20;
	static float bus1CutoffFromCc(int ccValue);

	void init(double sampleRate);
	void clearBusSums();
	MixBus& getBus(size_t busIndex);
	const MixBus& getBus(size_t busIndex) const;
	size_t getBusChannelCount(size_t busIndex) const;
	void setBusLowpassCutoff(size_t busIndex, float cutoffFreq);
	void processAll(float* master, size_t masterChannelCount);

private:
	MixBus buses[kBusCount];
};
