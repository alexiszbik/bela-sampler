#pragma once

class BitCrusher {
public:
    static constexpr int kMaxChannels = 2;
    static constexpr float kMaxHoldSamples = 64.f;

    void init(int channelCount = kMaxChannels);
    void setBitDepth(float bitDepth);
    void setRepeatRate(float rate);
    void reset();

    float process(float in, int channel);

private:
    void updateDerivedValues();

    int channelCount = kMaxChannels;
    float bitDepth = 16.f;
    float repeatRate = 1.f;
    float crushFactor = 32768.f;
    float holdSamples = 0.f;
    bool bypass = true;

    float lastSampleValue[kMaxChannels] = {};
    float holdCounter[kMaxChannels] = {};
};
