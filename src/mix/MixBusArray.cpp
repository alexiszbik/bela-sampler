#include "MixBusArray.h"

#include "FilterMixBus.h"
#include "FXMixBus.h"
#include "TomMixBus.h"
#include "SnareMixBus.h"
#include "HatMixBus.h"

#include <utility>

void MixBusArray::init(double sampleRate) {
	MixBusRoute masterRoute;
	masterRoute.mono = false;
	masterRoute.outputChannel0 = 4;
	masterRoute.outputChannel1 = 5;
	buses[kBusMaster] = std::make_unique<MixBusBase>();
	buses[kBusMaster]->init(sampleRate, masterRoute);

	MixBusRoute sampleRoute;
	sampleRoute.mono = false;
	//will be 0 & 1
	sampleRoute.outputChannel0 = 4;
	sampleRoute.outputChannel1 = 5;
	buses[kBusSample] = std::make_unique<FXMixBus>();
	buses[kBusSample]->init(sampleRate, sampleRoute);

	MixBusRoute kickRoute;
	kickRoute.mono = true;
	kickRoute.outputChannel0 = 0;
	buses[kBusKick] = std::make_unique<FilterMixBus>();
	buses[kBusKick]->init(sampleRate, kickRoute);

	MixBusRoute snareRoute;
	snareRoute.mono = true;
	snareRoute.outputChannel0 = 1;
	buses[kBusSnare] = std::make_unique<SnareMixBus>();
	buses[kBusSnare]->init(sampleRate, snareRoute);

	MixBusRoute tomsRoute;
	tomsRoute.mono = true;
	tomsRoute.outputChannel0 = 2;
	buses[kBusToms] = std::make_unique<TomMixBus>();
	buses[kBusToms]->init(sampleRate, tomsRoute);

	MixBusRoute hatsRoute;
	hatsRoute.mono = true;
	hatsRoute.outputChannel0 = 3;
	buses[kBusHats] = std::make_unique<HatMixBus>();
	buses[kBusHats]->init(sampleRate, hatsRoute);
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
			bus->processAndMixTo(master, masterChannelCount);
		}
	}
}
