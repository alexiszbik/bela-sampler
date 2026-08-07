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
    centerDelayMs = clampf(centerDelayMsIn, 0.1f, kMaxDelayMs - kMaxDepthMs);
}

void Flanger::reset() {
    delayLine.clear();
    lfo.reset();
    updateDelayTime();
}

void Flanger::updateDelayTime() {
    delayMs = clampf(centerDelayMs, 0.1f, kMaxDelayMs - 0.1f);
}

float Flanger::process(float in, int channel) {
    if (channel < 0 || channel >= channelCount) {
        return in;
    }

    if (mix <= 0.f) {
        return in;
    }

    if (channel == 0) {
        const float lfoValue = lfo.process();
        delayMs = clampf(centerDelayMs + depthMs * lfoValue, 0.1f, kMaxDelayMs - 0.1f);
    }

    float sampleBuf[1] = {in};
    delayLine.process(sampleBuf, 1, channel, &delayMs, &feedback, true, false);

    const float wet = sampleBuf[0];
    return in * (1.f - mix) - wet * mix;
}
