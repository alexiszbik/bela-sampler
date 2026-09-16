#include "MixBusBase.h"
#include "PanHelper.h"

void MixBusBase::init(double sampleRate, const MixBusRoute& route) {
	(void)sampleRate;

	switch (route.format)
	{
	case MixBusRoute::kStereo:
		channelCount = 2u;
		break;

	case MixBusRoute::kQuad:
		channelCount = 4u;
		break;
	
	default:
		channelCount = 1u;
		break;
	}

	
	outputChannel0 = route.outputChannel0;
	outputChannel1 = route.outputChannel1;
	outputChannel2 = route.outputChannel2;
	outputChannel3 = route.outputChannel3;

	clearSum();
}

void MixBusBase::clearSum() {
	for(size_t channel = 0; channel < channelCount; ++channel) {
		sum[channel] = 0.f;
	}
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

void MixBusBase::processEffects(float lfoBuf) {
	/* tremolo */
	/*
	float tremolo = (lfoBuf + 1.f) / 2.f;
	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] *= tremolo;
	}
	*/

	/* auto pan */
	/*
	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] *= panToRms(lfoBuf * 100, channel == 1);
	}
	*/
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

	if(channelCount > 2 && outputChannel2 < masterChannelCount) {
		master[outputChannel2] += sum[2];
	}

	if(channelCount > 3 && outputChannel3 < masterChannelCount) {
		master[outputChannel3] += sum[3];
	}
}

void MixBusBase::processAndMixTo(float* master, size_t masterChannelCount, float inLfo) {
	if(master == nullptr || masterChannelCount == 0) {
		return;
	}

	processEffects(inLfo);
	applyGain();
	mixToMaster(master, masterChannelCount);
}
