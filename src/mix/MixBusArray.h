#pragma once

#include "MixBus.h"

#include <cstddef>

class MixBusArray
{
public:
	static constexpr size_t kBusCount = 6;
	static constexpr size_t kMasterChannelCount = 8;

	void init(double sampleRate);
	void clearBusSums();
	MixBus& getBus(size_t busIndex);
	const MixBus& getBus(size_t busIndex) const;
	size_t getBusChannelCount(size_t busIndex) const;

	void setBusParameter(size_t busIndex, ParameterIndex parameterIndex, float value);

	void processAll(float* master, size_t masterChannelCount);

private:
	MixBus buses[kBusCount];
};
