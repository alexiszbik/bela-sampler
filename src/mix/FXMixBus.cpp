#include "FXMixBus.h"

#include "BufferMath.h"

void FXMixBus::init(double sampleRate, const MixBusRoute& route) {
	FilterMixBus::init(sampleRate, route);

	const float fsr = static_cast<float>(sampleRate);

	delayLine.init(channelCount, fsr);

	flangerSpeed.setValue(0.f);
	flangerLevel.setValue(0.f);
	flanger.init(channelCount, sampleRate);
	flanger.setDepth(0.5f);
	flanger.setFeedback(0.6f);
}

void FXMixBus::setParameterValue(ParameterIndex index, float value) {
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

		case FlangerSpeed:
			flangerSpeed.setValue(value * value * value);
			return;

		case FlangerLevel:
			flangerLevel.setValue(value);
			return;

		default:
			break;
	}

	FilterMixBus::setParameterValue(index, value);
}

void FXMixBus::processEffects(const TriLfo& lfo) {
	FilterMixBus::processEffects(lfo);

	if(flangerSpeed.valueHasChanged) {
		const float speed = flangerSpeed.getValue();
		flanger.setRate(0.05f + speed * 4.95f);
	}

	if(flangerLevel.valueHasChanged) {
		const float level = flangerLevel.getValue();
		flanger.setMix(level * level);
	}

	for(size_t channel = 0; channel < channelCount; ++channel) {
		sum[channel] = flanger.process(sum[channel], static_cast<int>(channel));
	}

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
