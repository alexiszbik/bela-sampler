#include "DelayMixBusSection.h"

#include "BufferMath.h"

#include <cmath>

void DelayMixBusSection::init(size_t channelCount, double sampleRate) {
	delayLine.init(static_cast<int>(channelCount), static_cast<float>(sampleRate));
}

bool DelayMixBusSection::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case DelayTime:
			freeDelayMs = value * 250.f + 10.f;
			return true;

		case DelayFeedback:
			feedback = value;
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

void DelayMixBusSection::setTempo(double tempo) {
	currentTempo = tempo;
}

void DelayMixBusSection::process(float* sum, size_t channelCount) {
	if(sum == nullptr || channelCount == 0) {
		return;
	}

	const double syncDelayMs = (60.0 / currentTempo) * 500.0;
	
	const float targetDelayMs = isDelaySync
		? static_cast<float>(syncDelayMs)
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

		sum[channel] = workBuf[0] * delayLevelValue;

		BufferMath::mul(workBuf, feedbackBuf, workBuf, frameCount);

		for(size_t i = 0; i < frameCount; ++i) {
			workBuf[i] += bufferIn[channel][i];
		}

		delayLine.write(workBuf, frameCount, static_cast<int>(channel));

		for(size_t i = 0; i < frameCount; ++i) {
			sum[channel] += bufferIn[channel][i];
		}
	}
}
