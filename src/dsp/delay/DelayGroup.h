

#pragma once

template <class T> class DelayGroup {
private:
    size_t channelCount;
    float sampleRate;
    
public:
    struct Dly {
        Dly(float maxDelayTime) {
            delayLine = new T(maxDelayTime);
        }
        
        T *delayLine;
        float timeRef;
        Buffer timeBuf;
    };
    
    std::vector<Dly*> lines;
    size_t count;
    
public:
    
    DelayGroup<T>(float* inDelayTimes, size_t inCount, float maxDelayTime) {
        count = inCount;
        
        for (size_t i = 0; i < count; i++) {
            lines.push_back(new Dly(maxDelayTime));
            lines.back()->timeRef = inDelayTimes[i];
            BufferMath::fill(lines.back()->timeBuf, inDelayTimes[i], MAX_BUFFER_SIZE);
        }
    }
    
    ~DelayGroup<T>() {
        for (size_t i = 0; i < count; i++) {
            delete lines[i]->delayLine;
            delete lines[i];
        }
    }
    
    virtual void init(int inChannelCount, double inSampleRate) {
        this->channelCount = inChannelCount;
        this->sampleRate = inSampleRate;
        
        for (size_t i = 0; i < count; i++) {
            lines.at(i)->delayLine->init(inChannelCount, sampleRate);
        }

        clear();
    }
    
    void clear() {
        for (size_t i = 0; i < count; i++) {
            lines.at(i)->delayLine->clear();
        }
    }
};
