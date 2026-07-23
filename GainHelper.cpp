#include "GainHelper.h"

#include <cmath>

float dBtoRMS(float dB) {
	return static_cast<float>(std::pow(10.0, static_cast<double>(dB) / 20.0));
}
