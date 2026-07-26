#include "MixBusArray.h"

#include <cmath>

static constexpr float kMixBus1MinCutoffHz = 80.f;
static constexpr float kMixBus1MaxCutoffHz = 16000.f;

float MixBusArray::bus1CutoffFromCc(int ccValue) {
	if(ccValue <= 0) {
		return kMixBus1MinCutoffHz;
	}

	if(ccValue >= 127) {
		return kMixBus1MaxCutoffHz;
	}

	const float normalized = static_cast<float>(ccValue) / 127.f;
	const float ratio = kMixBus1MaxCutoffHz / kMixBus1MinCutoffHz;
	return kMixBus1MinCutoffHz * static_cast<float>(std::pow(static_cast<double>(ratio), static_cast<double>(normalized)));
}

void MixBusArray::init(double sampleRate) {
	MixBusRoute bus0;
	bus0.mono = false;
	bus0.outputChannel0 = 0;
	bus0.outputChannel1 = 1;
	buses[0].init(sampleRate, bus0);

	MixBusRoute bus1;
	bus1.mono = false;
	bus1.outputChannel0 = 2;
	bus1.outputChannel1 = 3;
	buses[1].init(sampleRate, bus1);

	MixBusRoute bus3;
	bus3.mono = true;
	bus3.outputChannel0 = 4;
	buses[2].init(sampleRate, bus3);

	MixBusRoute bus4;
	bus4.mono = true;
	bus4.outputChannel0 = 5;
	buses[3].init(sampleRate, bus4);

	MixBusRoute bus5;
	bus5.mono = true;
	bus5.outputChannel0 = 6;
	buses[4].init(sampleRate, bus5);

	MixBusRoute bus6;
	bus6.mono = true;
	bus6.outputChannel0 = 7;
	buses[5].init(sampleRate, bus6);
}

void MixBusArray::clearBusSums() {
	for(MixBus& bus : buses) {
		bus.clearSum();
	}
}

MixBus& MixBusArray::getBus(size_t busIndex) {
	return buses[busIndex];
}

const MixBus& MixBusArray::getBus(size_t busIndex) const {
	return buses[busIndex];
}

size_t MixBusArray::getBusChannelCount(size_t busIndex) const {
	if(busIndex >= kBusCount) {
		return 0;
	}

	return buses[busIndex].getChannelCount();
}

void MixBusArray::setBusLowpassCutoff(size_t busIndex, float cutoffFreq) {
	if(busIndex >= kBusCount) {
		return;
	}

	buses[busIndex].setLowpassCutoff(cutoffFreq);
}

void MixBusArray::processAll(float* master, size_t masterChannelCount) {
	for(MixBus& bus : buses) {
		bus.processAndMixTo(master, masterChannelCount);
	}
}
