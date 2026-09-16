#include "SamplerVoice.h"

#include "GainHelper.h"
#include "PitchHelper.h"
#include "PanHelper.h"

void SamplerVoice::init(double sampleRate) {
	player.init(sampleRate);
}

void SamplerVoice::playOn(const Program::Slot& slot, int velocity) {
	if(slot.sample == nullptr) {
		return;
	}

	const bool loop = slot.mode == Program::SlotMode::Gate;
	const bool isGranular = slot.playMode == Program::SlotPlayMode::Granular;
	const float pitchSpeed = semitonesToPlaybackSpeed(slot.pitchSemitones);
	const float velocityGain = static_cast<float>(velocity) / 127.f;
	gain = velocityGain * velocityGain * dBtoRMS(slot.volumeDb);

	busIndex = slot.bus;
	dispatch = slot.dispatch;
	isMonoSample = slot.sample->getChannelCount() <= 1;

	balance[0] = panToRms(slot.pan, false);
	balance[1] = panToRms(slot.pan, true);

	player.setSample(slot.sample);
	player.setLoop(loop);
	player.setReversed(slot.reversed);
	player.setPlayMode(isGranular ? SamplePlayer::Granular : SamplePlayer::Normal);

	if(isGranular) {
		player.setGranularSpeed(slot.granularSpeed);
		player.setGranularPitch(pitchSpeed);
	} else {
		player.setSpeed(pitchSpeed);
	}

	player.trigger();

	if(slot.muteGroup != MuteGroup::None) {
		setActiveSlot(slot.id);
	}
}

void SamplerVoice::stop() {
	player.stop();
	clearActiveSlot();
}

void SamplerVoice::setVoiceBinding(const VoiceBinding& binding) {
	voiceBinding = binding;
}

void SamplerVoice::clearVoiceBinding() {
	voiceBinding = VoiceBinding{};
}

void SamplerVoice::setActiveSlot(size_t slotId) {
	voiceBinding.activeSlotId = slotId;
}

void SamplerVoice::clearActiveSlot() {
	voiceBinding.activeSlotId = VoiceBinding::kInvalidSlot;
}

void SamplerVoice::mixToStereoPair(float* sum, size_t leftChannel, size_t rightChannel, float left, float right, bool isMono) const {
	sum[leftChannel] += left;
	sum[rightChannel] += isMono ? left : right;
}

void SamplerVoice::mixDryToSum(float* sum, size_t sumChannelCount) {
	const float left = dry[0] * gain * balance[0];
	const float right = dry[1] * gain * balance[1];

	if(sumChannelCount < 4) {
		sum[0] += left;
		if(sumChannelCount > 1) {
			sum[1] += isMonoSample ? left : right;
		}
		return;
	}

	switch(dispatch) {
		case SlotDispatch::Rear:
			mixToStereoPair(sum, 2, 3, left, right, isMonoSample);
			break;


		case SlotDispatch::All:
			if(isMonoSample) {
				const float mono = dry[0] * gain;
				for(size_t channel = 0; channel < 4; ++channel) {
					sum[channel] += mono;
				}
			} else {
				mixToStereoPair(sum, 0, 1, left, right, false);
				mixToStereoPair(sum, 2, 3, left, right, false);
			}
			break;

		case SlotDispatch::Front:
		default:
			mixToStereoPair(sum, 0, 1, left, right, isMonoSample);
			break;
	}
}

void SamplerVoice::nextSamples(float* sum, size_t sumChannelCount) {
	dry[0] = 0.f;
	dry[1] = 0.f;

	player.nextSamples(dry, kMaxChannels);
	mixDryToSum(sum, sumChannelCount);
}
