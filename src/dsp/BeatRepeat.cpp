#include "BeatRepeat.h"

#include <algorithm>

void BeatRepeat::init(int inChannelCount, double inSampleRate) {
    channelCount = std::min(inChannelCount, kMaxChannels);
    sampleRate = inSampleRate;

    repeatSize = sampleRate*currentRate;

    memory = (float**)malloc(channelCount*sizeof(float*));
    for (int c = 0; c < channelCount; c++) {
        memory[c] = (float*)malloc(kMaxMemSize*sizeof(float));
        for (int i = 0; i < kMaxMemSize; i++) {
            memory[c][i] = 0;
        }
    }
}

void BeatRepeat::setState(bool newState) {
    if (state != newState) {
        state = newState;
        writeIdx = 0;
        readIdx = 0;
    }
}

void BeatRepeat::setRepeatRate(float value) {
    const int rateCount = rateList.size();
    int frate = static_cast<int>(floorf(value*rateCount));
    if (frate >= (rateCount - 1)) frate = rateCount-1;
    float newRate = rateList[frate];
    if (newRate != currentRate) {
        currentRate = newRate;
        updateRepeatSize();
    }

}

void BeatRepeat::setTempo(double tempo) {
    this->currentTempo = tempo;
    updateRepeatSize();
}

void BeatRepeat::updateRepeatSize() {
    double halfLength = 60.0/currentTempo * 2.0;
    repeatSize = fmin(currentRate * sampleRate * halfLength, kMaxMemSize);
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