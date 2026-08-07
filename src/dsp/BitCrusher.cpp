#include "BitCrusher.h"

#include <algorithm>
#include <cmath>

inline float clampf(float value, float min, float max) {
    if (value < min) value = min;
    if (value > max) value = max;
    return value;
}


void BitCrusher::init(int channelCount) {
    this->channelCount = std::min(channelCount, kMaxChannels);
    setBitDepth(16.f);
    setRepeatRate(1.f);
    reset();
}

void BitCrusher::setBitDepth(float inBitDepth) {
    bitDepth = inBitDepth;
    updateDerivedValues();
}

void BitCrusher::setRepeatRate(float rate) {
    repeatRate = clampf(rate, 0.f, 1.f);
    updateDerivedValues();
}

void BitCrusher::updateDerivedValues() {
    bypass = (bitDepth >= 16.f && repeatRate >= 1.f);

    const float clampedDepth = clampf(bitDepth - 1.f, 0.f, 15.f);
    crushFactor = powf(2.f, clampedDepth);
    holdSamples = clampf(truncf(64.f - repeatRate * 64.f), 0.f, 64.f);
}

void BitCrusher::reset() {
    for (int i = 0; i < kMaxChannels; i++) {
        lastSampleValue[i] = 0.f;
        holdCounter[i] = 0.f;
    }
}

float BitCrusher::process(float in, int channel) {
    if (bypass || channel < 0 || channel >= channelCount) {
        return in;
    }

    if (holdCounter[channel] > 0.f) {
        holdCounter[channel] -= 1.f;
    } else {
        lastSampleValue[channel] = truncf(in * crushFactor) / crushFactor;
        holdCounter[channel] = holdSamples;
    }

    return lastSampleValue[channel];
}
