#include "SamplerVoice.h"

#include "GainHelper.h"
#include "PitchHelper.h"

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
	busIndex = static_cast<size_t>(slot.bus);

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

void SamplerVoice::mixDryToSum(float* sum, size_t channelCount) {
	for(size_t channel = 0; channel < channelCount; channel++) {
		sum[channel] += dry[channel] * gain;
	}
}

void SamplerVoice::nextSamples(float* sum, size_t channelCount) {
	dry[0] = 0.f;
	dry[1] = 0.f;
	player.nextSamples(dry, channelCount);
	mixDryToSum(sum, channelCount);
}
