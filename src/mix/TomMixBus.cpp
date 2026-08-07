#include "TomMixBus.h"

#include "BufferMath.h"

void TomMixBus::init(double sampleRate, const MixBusRoute& route) {
	FilterMixBus::init(sampleRate, route);
	delayLine.init(static_cast<int>(channelCount), static_cast<float>(sampleRate));
}

void TomMixBus::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case DelayTime:
			delayTime.setValue(value * 250.f + 10.f);
			return;

		case DelayFeedback:
			feedback = value;
			return;

		case DelayLevel:
			delayLevel.setValue(value * value);
			return;

		default:
			break;
	}

	FilterMixBus::setParameterValue(index, value);
}

void TomMixBus::processEffects() {
	FilterMixBus::processEffects();

	const float delayLevelValue = delayLevel.getAndStep();
	float t = delayTime.getAndStep();

	float* timeBuf = &t;
	float* feedbackBuf = &feedback;
	const size_t frameCount = 1;

	float* out = sum;

	float bufferIn[2][1];

	for(size_t channel = 0; channel < channelCount; channel++) {
		bufferIn[channel][0] = sum[channel];
	}

	for(size_t channel = 0; channel < channelCount; channel++) {
		delayLine.process(workBuf, frameCount, channel, timeBuf, nullptr, false, true);

		for(size_t i = 0; i < frameCount; i++) {
			out[i * channelCount + channel] = workBuf[i] * delayLevelValue;
		}

		BufferMath::mul(workBuf, feedbackBuf, workBuf, frameCount);

		for(size_t i = 0; i < frameCount; i++) {
			workBuf[i] += bufferIn[channel][i];
		}

		delayLine.write(workBuf, frameCount, channel);

		for(size_t i = 0; i < frameCount; i++) {
			out[i * channelCount + channel] += bufferIn[channel][i];
		}
	}
}
