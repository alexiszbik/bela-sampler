#pragma once

#include <string>

enum MixBusIndex {
	kBusMaster = 0,
	kBusSample = 1,
	kBusKick = 2,
	kBusSnare = 3,
	kBusToms = 4,
	kBusHats = 5,
	kBusCount = 6,
};

bool mixBusIndexFromNickname(const std::string& name, MixBusIndex& outIndex);
const char* mixBusNickname(MixBusIndex index);
