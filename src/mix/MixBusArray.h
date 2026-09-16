#pragma once

#include "MixBusBase.h"
#include "MixBusNames.h"
#include "TriLfo.h"

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
	template<typename TBus>
	void initBus(MixBusIndex busIndex, double sampleRate, const MixBusRoute& route);

	template<typename TBus>
	void createMonoBus(MixBusIndex busIndex, double sampleRate, size_t outputChannel);

	template<typename TBus>
	void createStereoBus(MixBusIndex busIndex, double sampleRate, size_t outputChannel0, size_t outputChannel1);

	std::unique_ptr<MixBusBase> buses[kBusCount];
	TriLfo lfo;
};
