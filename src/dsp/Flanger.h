#pragma once

#include "DelayLine.h"
#include "TriLfo.h"

class Flanger {
public:
    static constexpr int kMaxChannels = 2;
    static constexpr float kMaxDelayMs = 40.f;
    static constexpr float kMaxDepthMs = 5.f;

    void init(int channelCount = kMaxChannels, double sampleRate = 44100.0);
    void setRate(float frequencyHz);
    void setDepth(float depth);
    void setFeedback(float feedback);
    void setMix(float mix);
    void setCenterDelayMs(float centerDelayMs);
    void reset();

    float process(float in, int channel);

private:
    void updateDelayTime();

    DelayLine delayLine{kMaxDelayMs};
    TriLfo lfo;

    int channelCount = kMaxChannels;
    float centerDelayMs = 4.f;
    float depthMs = 3.f;
    float feedback = 0.875f;
    float mix = 0.5f;

    float delayMs = 2.f;
};
