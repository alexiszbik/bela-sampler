#include "DelayMixBusSection.h"

#include "BufferMath.h"

#include <cmath>

/** Soft Limiting function ported extracted from pichenettes/stmlib */
inline float SoftLimit(float x)
{
    return x * (27.f + x * x) / (27.f + 9.f * x * x);
}

/** Soft Clipping function extracted from pichenettes/stmlib */
inline float SoftClip(float x)
{
    if(x < -3.0f)
        return -1.0f;
    else if(x > 3.0f)
        return 1.0f;
    else
        return SoftLimit(x);
}


void DelayMixBusSection::init(size_t channelCount, double sampleRate) {
	delayLine.init(static_cast<int>(channelCount), static_cast<float>(sampleRate));
	for(size_t channel = 0; channel < kMaxChannels; channel++) {
		fbkFilter[channel].init(sampleRate);
	}
	fbkFilter->setLowpass(2800, 0.8f);
}

bool DelayMixBusSection::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case DelayTime: {
			freeDelayMs = value * 250.f + 10.f;
			setDelayRate(value);
			return true;
		}

		case DelayFeedback:
			feedback = value * 1.1;
			return true;

		case DelayLevel:
			delayLevel.setValue(value * value);
			return true;

		case DelaySync:
			isDelaySync = value > 0.5f;
			return true;

		default:
			return false;
	}
}

void DelayMixBusSection::setDelayRate(float value) {
	if (value >= 0.75) {
		currentRate = 1.f;
	} else if (value >= 0.5) {
		currentRate = 0.75f;
	} else if (value >= 0.25) {
		currentRate = 0.5f; 
	} else {
		currentRate = 0.25f;
	}

}

void DelayMixBusSection::setTempo(double tempo) {
	currentTempo = tempo;
}

void DelayMixBusSection::process(float* sum, size_t channelCount) {
	if(sum == nullptr || channelCount == 0) {
		return;
	}

	const float syncDelayMs = (60.0 / currentTempo) * 1000.0 * currentRate;
	
	const float targetDelayMs = isDelaySync
		? syncDelayMs
		: freeDelayMs;

	delayTime.setValue(std::fmin(targetDelayMs, kMaxDelayMs));

	const float delayLevelValue = delayLevel.getAndStep();
	float t = delayTime.getAndStep();

	float* timeBuf = &t;
	float* feedbackBuf = &feedback;
	const size_t frameCount = 1;

	float bufferIn[kMaxChannels][1] = {};

	for(size_t channel = 0; channel < channelCount; ++channel) {
		bufferIn[channel][0] = sum[channel];
	}

	for(size_t channel = 0; channel < channelCount; ++channel) {
		delayLine.process(workBuf, frameCount, static_cast<int>(channel), timeBuf, nullptr, false, true);

		sum[channel] = workBuf[0];

		BufferMath::mul(workBuf, feedbackBuf, workBuf, frameCount);

		for(size_t i = 0; i < frameCount; ++i) {
			workBuf[i] += bufferIn[channel][i] * delayLevelValue;
			workBuf[i] = SoftClip(workBuf[i]);
			workBuf[i] = fbkFilter[channel].process(workBuf[i]);
		}

		delayLine.write(workBuf, frameCount, static_cast<int>(channel));

		for(size_t i = 0; i < frameCount; ++i) {
			sum[channel] += bufferIn[channel][i];
		}
	}
}
