

#pragma once

#include "DelayRead.h"
#include "DelayWrite.h"
#include "Buffer.h"
#include "BufferMath.h"

#include <cstring>

class DelayLine {

private:
    DelayWrite* writers;
    DelayRead* readers;
    
    float maxDelayTime = 0;

    Buffer delayOutBuf = 0.f;
    Buffer workBuf = 0.f;
protected:
    int channelCount;
    double sampleRate;
    
protected:
    virtual void preFeedbackProcess(float* dataIn, size_t n, size_t channel) {
    };
    
public:
    DelayLine(float maxDelayTimeInMs) {
        maxDelayTime = maxDelayTimeInMs;
    }

    virtual ~DelayLine() = default;
    
    virtual void init(int inChannelCount, double inSampleRate) {

        channelCount = inChannelCount;
        sampleRate = inSampleRate;
        
        writers = new DelayWrite[channelCount];
        readers = new DelayRead[channelCount];
        
        for (int i = 0; i < channelCount; i++) {
            writers[i].init(maxDelayTime, sampleRate);
            readers[i].init(sampleRate);
        }

        clear();
    }
    
    void clear()  {
        for (int i = 0; i < channelCount; i++) {
            writers[i].clear();
        }
    }
    
    void write(float* dataIn, size_t n, size_t channel) {
        writers[channel].process(dataIn, (int)n);
    }
    
    void process(float* dataIn, size_t n, size_t channel, float* timeInMs, float* feedback, bool reinject = true, bool wetOnly = false) {

        BufferMath::mul_s(timeInMs, 0.001f, workBuf, n);
        
        //Get data from delay
        readers[channel].process((float*)workBuf, delayOutBuf, &(writers[channel]), (int)n);
        
        if (feedback) {
            memcpy(workBuf, delayOutBuf, sizeof(float)*n);
            
            preFeedbackProcess(workBuf, n, channel);

            BufferMath::mul_add(workBuf, feedback, dataIn, workBuf, n);
        
        } else { // no feedback? give the line only
            if(wetOnly) {
                memcpy(workBuf, delayOutBuf, sizeof(float)*n);
            } else {
                memcpy(workBuf, dataIn, sizeof(float)*n);
            }
        }
        
        //Reinject in delay
        if (reinject) {
            write(workBuf, n, channel);
            memcpy(dataIn, delayOutBuf, sizeof(float)*n);
        } else {
            memcpy(dataIn, workBuf, sizeof(float)*n);
        }
    }
    
};


