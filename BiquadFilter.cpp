#include "BiquadFilter.h"

#include <cmath>

void BiquadFilter::init(float inSampleRate) {
	sampleRate = inSampleRate;
}

void BiquadFilter::setBandpass(float centerFreq, float q) {
	const float w0 = 2.f * static_cast<float>(M_PI) * centerFreq / sampleRate;
	const float alpha = std::sin(w0) / (2.f * q);
	const float cosw0 = std::cos(w0);

	b0 = alpha;
	b1 = 0.f;
	b2 = -alpha;
	a0 = 1.f + alpha;
	a1 = -2.f * cosw0;
	a2 = 1.f - alpha;

	normalize();
}

void BiquadFilter::setLowpass(float cutoffFreq, float q) {
	const float w0 = 2.f * static_cast<float>(M_PI) * cutoffFreq / sampleRate;
	const float alpha = std::sin(w0) / (2.f * q);
	const float cosw0 = std::cos(w0);

	b0 = (1.f - cosw0) / 2.f;
	b1 = 1.f - cosw0;
	b2 = (1.f - cosw0) / 2.f;
	a0 = 1.f + alpha;
	a1 = -2.f * cosw0;
	a2 = 1.f - alpha;

	normalize();
}

void BiquadFilter::setHighpass(float cutoffFreq, float q) {
	const float w0 = 2.f * static_cast<float>(M_PI) * cutoffFreq / sampleRate;
	const float alpha = std::sin(w0) / (2.f * q);
	const float cosw0 = std::cos(w0);

	b0 = (1.f + cosw0) / 2.f;
	b1 = -(1.f + cosw0);
	b2 = (1.f + cosw0) / 2.f;
	a0 = 1.f + alpha;
	a1 = -2.f * cosw0;
	a2 = 1.f - alpha;

	normalize();
}

float BiquadFilter::process(float input) {
	const float output = b0 * input + z1;
	z1 = b1 * input - a1 * output + z2;
	z2 = b2 * input - a2 * output;
	return output;
}

void BiquadFilter::reset() {
	z1 = 0.f;
	z2 = 0.f;
}

void BiquadFilter::normalize() {
	b0 /= a0;
	b1 /= a0;
	b2 /= a0;
	a1 /= a0;
	a2 /= a0;
	a0 = 1.f;
}
