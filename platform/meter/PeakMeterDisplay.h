#pragma once

#include <algorithm>
#include <cmath>
#include <string>

namespace PeakMeterDisplay {
constexpr float kMinDb = -48.f;
constexpr float kMaxDb = 0.f;
constexpr int kRefreshHz = 30;

enum class LevelColour {
	Green,
	Yellow,
	Red
};

inline float amplitudeToDb(float amplitude) {
	if(amplitude <= 0.f) {
		return kMinDb;
	}

	return 20.f * std::log10(amplitude);
}

inline float dbToNormalised(float db) {
	return std::clamp((db - kMinDb) / (kMaxDb - kMinDb), 0.f, 1.f);
}

inline float rmsToNormalised(float rms) {
	return dbToNormalised(amplitudeToDb(rms));
}

inline float peakToNormalised(float peak) {
	return dbToNormalised(amplitudeToDb(peak));
}

inline std::string formatPeakDb(float peak) {
	if(peak <= 0.f) {
		return std::to_string(static_cast<int>(kMinDb)) + ".0";
	}

	const float db = std::clamp(amplitudeToDb(peak), kMinDb, kMaxDb + 6.f);
	char buffer[16];
	std::snprintf(buffer, sizeof(buffer), "%.1f", db);
	return buffer;
}

inline LevelColour colourForLevel(float normalisedLevel) {
	if(normalisedLevel >= 0.92f) {
		return LevelColour::Red;
	}

	if(normalisedLevel >= 0.75f) {
		return LevelColour::Yellow;
	}

	return LevelColour::Green;
}
}
