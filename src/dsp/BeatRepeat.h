#pragma once



class BeatRepeat {
public:
    static constexpr int kMaxChannels = 2;
    static constexpr int kMaxMemSize = 48000*4;

    void init(int inChannelCount = kMaxChannels, double inSampleRate = 44100.0);
    float process(float in, int channel);

    void setState(bool newState);

private:
    bool state = false;

    int channelCount = kMaxChannels;
    double sampleRate;

    //temp
    int repeatSize;

    int writeIdx = 0;
    int readIdx = 0;
    float memory[2][kMaxMemSize];
};