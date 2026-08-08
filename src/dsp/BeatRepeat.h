#pragma once


#include <array>
#include <cmath>

class BeatRepeat {
public:
    static constexpr int kMaxChannels = 2;
    static constexpr int kMaxMemSize = 48000;

    void init(int inChannelCount = kMaxChannels, double inSampleRate = 44100.0);
    float process(float in, int channel);

    void setState(bool newState);
    void setRepeatRate(float value);

private:
    bool state = false;

    int channelCount = kMaxChannels;
    double sampleRate;

    int repeatSize = 0;

    int writeIdx = 0;
    int readIdx = 0;
    float memory[2][kMaxMemSize];

    float currentRate = 0.125f;

    std::array<float, 8> rateList = {1.f, 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f, 0.015625f, 0.0078125f};
};