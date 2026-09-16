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

	MixBusRoute masterRouteRear;
	masterRouteRear.mono = false;
	masterRouteRear.outputChannel0 = 6;
	masterRouteRear.outputChannel1 = 7;
	buses[kBusMasterRear] = std::make_unique<MixBusBase>();
	buses[kBusMasterRear]->init(sampleRate, masterRouteRear);

	MixBusRoute sampleRoute;
	sampleRoute.mono = false;
	sampleRoute.outputChannel0 = 4;
	sampleRoute.outputChannel1 = 5;
	buses[kBusSample] = std::make_unique<FXMixBus>();
	buses[kBusSample]->init(sampleRate, sampleRoute);

	MixBusRoute sampleRouteRear;
	sampleRouteRear.mono = false;
	sampleRouteRear.outputChannel0 = 6;
	sampleRouteRear.outputChannel1 = 7;
	buses[kBusSampleRear] = std::make_unique<FXMixBus>();
	buses[kBusSampleRear]->init(sampleRate, sampleRoute);

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
