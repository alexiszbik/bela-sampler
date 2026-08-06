#include "MixBus.h"

#include "BufferMath.h"

void MixBus::init(double sampleRate, const MixBusRoute& route) {
	channelCount = route.mono ? 1u : 2u;
	outputChannel0 = route.outputChannel0;
	outputChannel1 = route.outputChannel1;

	const float fsr = static_cast<float>(sampleRate);

	lowpassSection.init(fsr, MixBusFilterType::Lowpass, channelCount);
	highpassSection.init(fsr, MixBusFilterType::Highpass, channelCount);

	lowpassSection.setCutoffRatio(1.0f);
	highpassSection.setCutoffRatio(0.0f);

	lowpassSection.applyPending();
	highpassSection.applyPending();

	lowpassSection.reset();
	highpassSection.reset();

	delayLine.init(channelCount, fsr);

	clearSum();
}

void MixBus::clearSum() {
	sum[0] = 0.f;
	sum[1] = 0.f;
}

float* MixBus::getSum() {
	return sum;
}

void MixBus::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case Volume:
			volume.setValue(value*value); 
			break;

		case Mute:
			mute.setValue(value > 0.5f ? 0.f : 1.f);
			break;

		case LowPassCutoff:
			lowpassSection.setCutoffRatio(value);
			break;

		case HiPassCutoff:
			highpassSection.setCutoffRatio(value);
			break;

		case DelayTime:
			delayTime.setValue(value * 250.f + 10.f);
			break;

		case DelayFeedback:
			feedback = value;
			break;

		case DelayLevel:
			delayLevel.setValue(value*value);
			break;
	}
}

void MixBus::processAndMixTo(float* master, size_t masterChannelCount) {
	if(master == nullptr || masterChannelCount == 0) {
		return;
	}

	lowpassSection.applyPending();
	highpassSection.applyPending();

	float muteGain = mute.getAndStep();
	float volumeValue = volume.getAndStep();

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] = lowpassSection.process(channel, sum[channel]);
		sum[channel] = highpassSection.process(channel, sum[channel]);
	}

	/* DELAY */

	float delayLevelValue = delayLevel.getAndStep();

	float t = delayTime.getAndStep();

    float* timeBuf = &t;
    float* feedbackBuf = &feedback;
	size_t frameCount = 1;

	float* out = sum;

	float bufferIn[2][1];

	for(size_t channel = 0; channel < channelCount; channel++) {
		bufferIn[channel][0] = sum[channel];
	}

    for(size_t channel = 0; channel < channelCount; channel++)
    {
        delayLine.process(workBuf, frameCount, channel, timeBuf, nullptr, false, true);
        
        for(size_t i = 0; i < frameCount; i++)
        {
            out[i * channelCount + channel] = workBuf[i] * delayLevelValue;
        }
        
        BufferMath::mul(workBuf, feedbackBuf, workBuf, frameCount);

        for(size_t i = 0; i < frameCount; i++)
        {
            workBuf[i] += bufferIn[channel][i];
        }

        delayLine.write(workBuf, frameCount, channel);

        for(size_t i = 0; i < frameCount; i++)
        {
            out[i * channelCount + channel] += bufferIn[channel][i];
        }
    }

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] *= volumeValue * muteGain;
	}

	if(outputChannel0 < masterChannelCount) {
		master[outputChannel0] += sum[0];
	}

	if(channelCount > 1 && outputChannel1 < masterChannelCount) {
		master[outputChannel1] += sum[1];
	}
}
