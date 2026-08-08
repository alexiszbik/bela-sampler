#pragma once

#include "MixBusBase.h"
#include "MixBusNames.h"

#include <cstddef>
#include <memory>

class MixBusArray
{
public:
	static constexpr size_t kMasterChannelCount = 8;

	void init(double sampleRate);
	void clearBusSums();
	MixBusBase& getBus(MixBusIndex busIndex);
	void setBusParameter(MixBusIndex busIndex, ParameterIndex parameterIndex, float value);
	void processAll(float* master, size_t masterChannelCount);

private:
	std::unique_ptr<MixBusBase> buses[kBusCount];
};
