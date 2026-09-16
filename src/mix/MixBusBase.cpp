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

	for(size_t channel = 0; channel < kMaxChannels; ++channel) {
		outputChannels[channel] = route.outputChannels[channel];
	}

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

void MixBusBase::processEffects(const TriLfo& lfo) {
	/* tremolo */
	/*
	const float tremolo = (lfo.valueAtPhaseOffset(0.f) + 1.f) / 2.f;
	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] *= tremolo;
	}
	*/

	/* auto pan */

	
	//THIS DISTRIB is tooo focus
	/*
	if(channelCount >= 4) {
		for(size_t channel = 0; channel < channelCount; ++channel) {
			const float phaseOffset = 0.25f * static_cast<float>(channel);
			const float lfoValue = lfo.valueAtPhaseOffset(phaseOffset);
			sum[channel] *= lfoValue >= 0 ? lfoValue : 0.f;
		}
	}
	*/

	//THIS DISTRIB is tooo genereous
	/*
	if(channelCount >= 4) {
		for(size_t channel = 0; channel < channelCount; ++channel) {
			const float phaseOffset = 0.25f * static_cast<float>(channel);
			const float lfoValue = (lfo.valueAtPhaseOffset(phaseOffset) + 1.f) / 2.f;
			sum[channel] *= lfoValue;
		}
	}*/

	if(channelCount >= 4) {
		for(size_t channel = 0; channel < channelCount; ++channel) {
			const float phaseOffset = 0.25f * static_cast<float>(channel);
			const float lfoValue = (lfo.valueAtPhaseOffset(phaseOffset) + 1.f) / 2.f;
			sum[channel] *= lfoValue * lfoValue;
		}
	}
}

void MixBusBase::applyGain() {
	const float muteGain = mute.getAndStep();
	const float volumeValue = volume.getAndStep();

	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] *= volumeValue * muteGain;
	}
}

void MixBusBase::mixToMaster(float* master, size_t masterChannelCount) {
	for(size_t channel = 0; channel < channelCount; ++channel) {
		const size_t outputChannel = outputChannels[channel];
		if(outputChannel < masterChannelCount) {
			master[outputChannel] += sum[channel];
		}
	}
}

void MixBusBase::processAndMixTo(float* master, size_t masterChannelCount, const TriLfo& lfo) {
	if(master == nullptr || masterChannelCount == 0) {
		return;
	}

	processEffects(lfo);
	applyGain();
	mixToMaster(master, masterChannelCount);
}
