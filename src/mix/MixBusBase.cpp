#include "MixBusBase.h"

void MixBusBase::init(double sampleRate, const MixBusRoute& route) {
	(void)sampleRate;

	channelCount = route.mono ? 1u : 2u;
	outputChannel0 = route.outputChannel0;
	outputChannel1 = route.outputChannel1;

	clearSum();
}

void MixBusBase::clearSum() {
	sum[0] = 0.f;
	sum[1] = 0.f;
}

float* MixBusBase::getSum() {
	return sum;
}

void MixBusBase::setParameterValue(ParameterIndex index, float value) {
	switch(index) {
		case Volume:
			volume.setValue(value * value);
			break;

		case Mute:
			mute.setValue(value > 0.5f ? 0.f : 1.f);
			break;

		default:
			break;
	}
}

void MixBusBase::processEffects() {
}

void MixBusBase::applyGain() {
	const float muteGain = mute.getAndStep();
	const float volumeValue = volume.getAndStep();

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] *= volumeValue * muteGain;
	}
}

void MixBusBase::mixToMaster(float* master, size_t masterChannelCount) {
	if(outputChannel0 < masterChannelCount) {
		master[outputChannel0] += sum[0];
	}

	if(channelCount > 1 && outputChannel1 < masterChannelCount) {
		master[outputChannel1] += sum[1];
	}
}

void MixBusBase::processAndMixTo(float* master, size_t masterChannelCount) {
	if(master == nullptr || masterChannelCount == 0) {
		return;
	}

	processEffects();
	applyGain();
	mixToMaster(master, masterChannelCount);
}
