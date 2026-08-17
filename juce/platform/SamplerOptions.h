#pragma once

#include "MixBusNames.h"
#include "ProgramJson.h"

#include <juce_core/juce_core.h>

namespace SamplerOptions
{
inline juce::StringArray modeOptions() {
	return {
		ProgramJson::kModePoly,
		ProgramJson::kModeMono,
		ProgramJson::kModeGate
	};
}

inline juce::StringArray playModeOptions() {
	return {
		ProgramJson::kPlayModeNormal,
		ProgramJson::kPlayModeGranular
	};
}

inline juce::StringArray muteGroupOptions() {
	return {
		"-",
		ProgramJson::kMuteGroupA,
		ProgramJson::kMuteGroupB,
		ProgramJson::kMuteGroupC,
		ProgramJson::kMuteGroupD
	};
}

inline juce::StringArray busOptions() {
	juce::StringArray result;
	for(int i = kBusMaster; i < kBusCount; ++i) {
		result.add(juce::String(mixBusNickname(static_cast<MixBusIndex>(i))));
	}
	return result;
}
}
