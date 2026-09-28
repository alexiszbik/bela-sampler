#include "Flanger.h"

#include <algorithm>
#include <cmath>

namespace {
float clampf(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}
}

void Flanger::init(int channelCountIn, double sampleRate) {
    channelCount = std::min(channelCountIn, kMaxChannels);
    delayLine.init(channelCount, sampleRate);
    lfo.init(sampleRate);
    lfo.setFrequency(0.5f);

    phaseOffsetRatio = 1.f/channelCount;
    reset();
}

void Flanger::setRate(float frequencyHz) {
    lfo.setFrequency(frequencyHz);
}

void Flanger::setDepth(float depth) {
    depthMs = clampf(depth, 0.f, 1.f) * kMaxDepthMs;
}

void Flanger::setFeedback(float feedbackIn) {
    feedback = clampf(feedbackIn, 0.f, 1.f);
}

void Flanger::setMix(float mixIn) {
    mix = clampf(mixIn, 0.f, 1.f);
}

void Flanger::setCenterDelayMs(float centerDelayMsIn) {
    centerDelayMs = clampf(centerDelayMsIn, 0.1f, kMaxFanglerDelayMs - kMaxDepthMs);
}

void Flanger::reset() {
    delayLine.clear();
    lfo.reset();
    updateDelayTime();
}

void Flanger::updateDelayTime() {
    delayMs = clampf(centerDelayMs, 0.1f, kMaxFanglerDelayMs - 0.1f);
}

float Flanger::process(float in, int channel) {

    if (channel < 0 || channel >= channelCount) {
        return in;
    }

    if (mix <= 0.f) {
        return in;
    }

    if (channel == 0) {
        lfo.process();
    }

    const float phaseOffset = phaseOffsetRatio * static_cast<float>(channel);
	const float lfoValue = lfo.valueAtPhaseOffset(phaseOffset);

    delayMs = clampf(centerDelayMs + depthMs * lfoValue, 0.1f, kMaxFanglerDelayMs - 0.1f);

    float sampleBuf = in;
    delayLine.process(&sampleBuf, 1, channel, &delayMs, &feedback, true, false);

    const float wet = sampleBuf;
    const float halfpi = 1.57079633;

    const float dryW = std::cos(mix * halfpi);
    const float wetW = std::sin(mix * halfpi);
    return in * dryW - wet * wetW;
    //return in * (1.f - mix) - wet * mix;
}
