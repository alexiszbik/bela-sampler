#include "CutoffHelper.h"

#include <cmath>

float clampCutoffRatio(float ratio) {
	if(ratio < 0.f) {
		return 0.f;
	}

	if(ratio > 1.f) {
		return 1.f;
	}

	return ratio;
}

float cutoffRatioToHz(float ratio) {
	const float clampedRatio = clampCutoffRatio(ratio);
	const float logRatio = kMaxCutoffHz / kMinCutoffHz;
	return kMinCutoffHz * static_cast<float>(std::pow(static_cast<double>(logRatio), static_cast<double>(clampedRatio)));
}
