#include "PitchHelper.h"

#include <cmath>

float clampPitchSemitones(float semitones) {
	if(semitones < kMinPitchSemitones) {
		return kMinPitchSemitones;
	}

	if(semitones > kMaxPitchSemitones) {
		return kMaxPitchSemitones;
	}

	return semitones;
}

float semitonesToPlaybackSpeed(float semitones) {
	return static_cast<float>(std::pow(2.0, static_cast<double>(semitones) / 12.0));
}
