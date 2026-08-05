
#include "SmoothValue.h"

SmoothValue::SmoothValue(float value) {
    setImmediate(value);
}

void SmoothValue::setImmediate(float value) {
    goal = this->value = value;
    inverseSlope = 0.0;
    samplesRemaining = 0;
}

void SmoothValue::setValue(float value) {
    if (this->value != value) {
        this->value = value;
        valueChanged = true;
    }
}

void SmoothValue::dezipperCheck()
{
    if (valueChanged) {
        valueChanged = false;
        startRamp(value, rampDuration);
    }
}

void SmoothValue::startRamp(float newGoal, long duration) {
    if (duration == 0) {
        setImmediate(newGoal);
        return;
    }

    const float current = get();
    inverseSlope = (current - newGoal) / float(duration);
    goal = newGoal;
    samplesRemaining = duration;
}

float SmoothValue::get() {
    return inverseSlope * float(samplesRemaining) + goal;
}

float SmoothValue::getAndStep() {
    dezipperCheck();
    if (samplesRemaining > 0) {
        float currentValue = get();
        --samplesRemaining;
        return currentValue;
    }
    else {
        return goal;
    }
}

