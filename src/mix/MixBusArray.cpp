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

void MixBusArray::init(double sampleRate) {
	initBus<MixBusBase>(kBusMaster, sampleRate, MixBusRoute::quad(4, 5, 6, 7));
	initBus<FXMixBus>(kBusSample, sampleRate, MixBusRoute::quad(4, 5, 6, 7));
	initBus<FilterMixBus>(kBusKick, sampleRate, MixBusRoute::mono(0));
	initBus<SnareMixBus>(kBusSnare, sampleRate, MixBusRoute::mono(1));
	initBus<TomMixBus>(kBusToms, sampleRate, MixBusRoute::mono(2));
	initBus<HatMixBus>(kBusHats, sampleRate, MixBusRoute::mono(3));

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

	for(const std::unique_ptr<MixBusBase>& bus : buses) {
		if(bus != nullptr) {
			bus->processAndMixTo(master, masterChannelCount, lfo);
		}
	}

	lfo.process();
}
