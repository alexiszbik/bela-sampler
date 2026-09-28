
#pragma once

#include <cstddef>

class SmoothValue {
public:
    SmoothValue() {};

    SmoothValue(float value, int rampDuration = 800);
    ~SmoothValue() {};
  
    
    void setImmediate(float value);

    void setValue(float value);
    
    float getAndStep();
    
private:
    void startRamp(float newGoal, long duration);
    float get();
    void dezipperCheck();
    
private:
    float value = 0.0f;
    float goal = 0.0f;
    float inverseSlope = 0.0f;
    long samplesRemaining = 0;
    
    bool valueChanged = true;

    int rampDuration = 800;
};

