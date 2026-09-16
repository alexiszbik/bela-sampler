#pragma once

#include <string>

enum MixBusIndex {
	kBusMaster = 0,
    kBusMasterRear,
	kBusSample,
    kBusSampleRear,
	kBusKick,
	kBusSnare,
	kBusToms,
	kBusHats,
	kBusCount,
};

bool mixBusIndexFromNickname(const std::string& name, MixBusIndex& outIndex);
const char* mixBusNickname(MixBusIndex index);
bool isRear(MixBusIndex);
