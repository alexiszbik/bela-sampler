#include "PanHelper.h"

#include <cmath>

namespace {
	inline float clampf(float value, float min, float max) {
    	if (value < min) value = min;
    	if (value > max) value = max;
    	return value;
	}
}

float clampPan(float pan) {
	if(pan < kMinPan) {
		return pan;
	}

	if(pan > kMaxPan) {
		return pan;
	}

	return pan;
}

float panToRms(float pan, bool isRight) {
    if (isRight) {
        pan *= -1;
    }
    
    float balance = clampf(pan, 0, 100.f);
	balance = (100.f - balance)/100.f;
    
    return balance*balance;
}
