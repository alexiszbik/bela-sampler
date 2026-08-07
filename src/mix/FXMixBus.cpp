#include "FXMixBus.h"

#include "BufferMath.h"

void FXMixBus::init(double sampleRate, const MixBusRoute& route) {
	MixBusBase::init(sampleRate, route);

	const float fsr = static_cast<float>(sampleRate);

	lowpassSection.init(fsr, MixBusFilterType::Lowpass, channelCount);
	highpassSection.init(fsr, MixBusFilterType::Highpass, channelCount);

	lowpassSection.setCutoffRatio(1.0f);
	highpassSection.setCutoffRatio(0.0f);

	lowpassSection.applyPending();
	highpassSection.applyPending();

	lowpassSection.reset();
	highpassSection.reset();

	delayLine.init(static_cast<int>(channelCount), fsr);
	reverb.init(static_cast<int>(channelCount), fsr);

	rvbLfo.init(fsr);
	rvbLfo.setFrequency(0.5f);

	bitCrushRate.setValue(1.f);

	bitCrush.init(channelCount);
}

void FXMixBus::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case LowPassCutoff:
			lowpassSection.setCutoffRatio(value);
			return;

		case HiPassCutoff:
			highpassSection.setCutoffRatio(value);
			return;

		case DelayTime:
			delayTime.setValue(value * 250.f + 10.f);
			return;

		case DelayFeedback:
			feedback = value;
			return;

		case DelayLevel:
			delayLevel.setValue(value * value);
			return;

		case ReverbSend:
			reverbSendLevel.setValue(value * value);
			return;

		case BitCrushRate:
			bitCrushRate.setValue(value);
			return;

		default:
			break;
	}

	MixBusBase::setParameterValue(index, value);
}

void FXMixBus::processEffects() {
	if (bitCrushRate.valueHasChanged) {
		bitCrush.setRepeatRate(bitCrushRate.getValue());
	}
	
	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = bitCrush.process(sum[channel], channel);
	}

	lowpassSection.applyPending();
	highpassSection.applyPending();

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = lowpassSection.process(channel, sum[channel]);
		sum[channel] = highpassSection.process(channel, sum[channel]);
	}

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

	if(enableReverb) {
		float rvbLevel = reverbSendLevel.getAndStep();

		float lfoValue = rvbLfo.process() * 0.25f;

		reverb.setRoomSize(50.f, &lfoValue, frameCount);

		float inLeft = sum[0] * rvbLevel;
		float inRight = channelCount > 1 ? sum[1] : sum[0];
		inRight *= rvbLevel;

		reverb.process(&inLeft, &inRight, 1, reverbTime);

		sum[0] += inLeft;
		if (channelCount > 1) {
			sum[1] += inRight;
		}
		
	}
}
