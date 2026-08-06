#include "TriLfo.h"

#include <cmath>

float TriLfo::triangleFromPhase(float p) {
	return p < 0.5f ? (4.0f * p - 1.0f) : (3.0f - 4.0f * p);
}

void TriLfo::init(double sampleRateIn) {
	sampleRate = sampleRateIn;
	setFrequency(frequencyHz);
}

void TriLfo::setFrequency(float frequencyHzIn) {
	frequencyHz = frequencyHzIn;
	phaseIncrement = static_cast<float>(frequencyHz / sampleRate);
}

void TriLfo::setPhase(float phaseIn) {
	phase = phaseIn - std::floor(phaseIn);
}

void TriLfo::reset() {
	phase = 0.0f;
}

float TriLfo::process() {
	const float output = triangleFromPhase(phase);

	phase += phaseIncrement;
	if(phase >= 1.0f) {
		phase -= 1.0f;
	}

	return output;
}
