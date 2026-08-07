#pragma once

#include "MixBusBase.h"

#include <cstddef>
#include <memory>

class MixBusArray
{
public:
	static constexpr size_t kBusCount = 6;
	static constexpr size_t kMasterChannelCount = 8;

	void init(double sampleRate);
	void clearBusSums();
	MixBusBase& getBus(size_t busIndex);
	const MixBusBase& getBus(size_t busIndex) const;
	size_t getBusChannelCount(size_t busIndex) const;

	void setBusParameter(size_t busIndex, ParameterIndex parameterIndex, float value);

	void processAll(float* master, size_t masterChannelCount);

private:
	std::unique_ptr<MixBusBase> buses[kBusCount];
};
