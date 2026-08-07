#include "BeatRepeat.h"

void BeatRepeat::init(int inChannelCount, double inSampleRate) {
    channelCount = inChannelCount;
    sampleRate = inSampleRate;

    repeatSize = sampleRate*0.125;

}

void BeatRepeat::setState(bool newState) {
    if (state != newState) {
        state = newState;
        writeIdx = 0;
        readIdx = 0;
    }

}

float BeatRepeat::process(float in, int channel) {
    float out = in;
    if (state) {
        if (writeIdx < kMaxMemSize && writeIdx < repeatSize) {
             memory[channel][writeIdx] = in; //write in memory
             if (channel == (channelCount - 1)) {
                writeIdx++;
             }

        } else {
            out = memory[channel][readIdx];
            if (channel == (channelCount - 1)) {
                readIdx++;
                if (readIdx > repeatSize) {
                    readIdx = 0;
                }
            }
        }
    }
    return out;
}