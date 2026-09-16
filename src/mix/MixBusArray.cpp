#include "MixBusArray.h"

#include "FilterMixBus.h"
#include "FXMixBus.h"
#include "TomMixBus.h"
#include "SnareMixBus.h"
#include "HatMixBus.h"

template<typename TBus>
void MixBusArray::initBus(MixBusIndex busIndex, double sampleRate, const MixBusRoute& route) {
	buses[busIndex] = std::make_unique<TBus>();
	buses[busIndex]->init(sampleRate, route);
}

template<typename TBus>
void MixBusArray::createMonoBus(MixBusIndex busIndex, double sampleRate, size_t outputChannel) {
	MixBusRoute route;
	route.mono = true;
	route.outputChannel0 = outputChannel;
	initBus<TBus>(busIndex, sampleRate, route);
}

template<typename TBus>
void MixBusArray::createStereoBus(MixBusIndex busIndex, double sampleRate, size_t outputChannel0, size_t outputChannel1) {
	MixBusRoute route;
	route.mono = false;
	route.outputChannel0 = outputChannel0;
	route.outputChannel1 = outputChannel1;
	initBus<TBus>(busIndex, sampleRate, route);
}

template void MixBusArray::createMonoBus<FilterMixBus>(MixBusIndex, double, size_t);
template void MixBusArray::createMonoBus<SnareMixBus>(MixBusIndex, double, size_t);
template void MixBusArray::createMonoBus<TomMixBus>(MixBusIndex, double, size_t);
template void MixBusArray::createMonoBus<HatMixBus>(MixBusIndex, double, size_t);

template void MixBusArray::createStereoBus<MixBusBase>(MixBusIndex, double, size_t, size_t);
template void MixBusArray::createStereoBus<FXMixBus>(MixBusIndex, double, size_t, size_t);

void MixBusArray::init(double sampleRate) {
	createStereoBus<MixBusBase>(kBusMaster, sampleRate, 4, 5);
	createStereoBus<MixBusBase>(kBusMasterRear, sampleRate, 6, 7);
	createStereoBus<FXMixBus>(kBusSample, sampleRate, 4, 5);
	createStereoBus<FXMixBus>(kBusSampleRear, sampleRate, 6, 7);
	createMonoBus<FilterMixBus>(kBusKick, sampleRate, 0);
	createMonoBus<SnareMixBus>(kBusSnare, sampleRate, 1);
	createMonoBus<TomMixBus>(kBusToms, sampleRate, 2);
	createMonoBus<HatMixBus>(kBusHats, sampleRate, 3);

	lfo.init(sampleRate);
	lfo.setFrequency(0.25f);
}

void MixBusArray::clearBusSums() {
	for(const std::unique_ptr<MixBusBase>& bus : buses) {
		if(bus != nullptr) {
			bus->clearSum();
		}
	}
}

MixBusBase& MixBusArray::getBus(MixBusIndex busIndex) {
	return *buses[busIndex];
}

void MixBusArray::setBusParameter(MixBusIndex busIndex, ParameterIndex parameterIndex, float value) {
	if(busIndex < kBusMaster || busIndex >= kBusCount || buses[busIndex] == nullptr) {
		return;
	}

	buses[busIndex]->setParameterValue(parameterIndex, value);
}

void MixBusArray::processAll(float* master, size_t masterChannelCount) {

	float lfoValue = lfo.process();
	for(const std::unique_ptr<MixBusBase>& bus : buses) {
		if(bus != nullptr) {
			bus->processAndMixTo(master, masterChannelCount, lfoValue);
		}
	}
}
