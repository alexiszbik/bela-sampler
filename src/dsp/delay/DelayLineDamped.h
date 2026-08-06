
#pragma once

#include "DelayLine.h"
#include "BiquadFilter.h"

#include <vector>

//Damp algorithm is lowPass filter signal - signal

class DelayLineDamped : public DelayLine {
    
private:
    std::vector<BiquadFilter> lpFilter;
    Buffer workBuf;

public:

    DelayLineDamped(float maxDelayTimeInMs) : DelayLine(maxDelayTimeInMs) {
    }
    
    void init(int inChannelCount, double inSampleRate) override {
        DelayLine::init(inChannelCount, inSampleRate);
        
        for (size_t i = 0; i < inChannelCount; i++) {
            lpFilter.push_back(BiquadFilter());
        }
        
        for (size_t i = 0; i < inChannelCount; i++) {
            lpFilter[i].init(this->sampleRate);
            lpFilter[i].setLowpass(9000, 0.707f);
        }
    }
    
    void clear() {
        DelayLine::clear();
        lpFilter.clear();
    }
    
    void process(float* dataIn, size_t n, size_t channel, float* timeInMs, float* feedback, bool reinject = true) {
        DelayLine::process(dataIn, n, channel, timeInMs, feedback, reinject);
    }
    
    void preFeedbackProcess(float* dataIn, size_t n, size_t channel) override {
        
        memcpy(workBuf, dataIn, sizeof(float)*n);
        
        for (size_t i = 0; i < n; i++) {
            workBuf[i] = lpFilter[channel].process(workBuf[i]);
        }
        
        BufferMath::sub(workBuf, dataIn, workBuf, n);
        
        BufferMath::mul_s_add(workBuf, 0.35, dataIn, dataIn, n);
    }
};

