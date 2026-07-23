#include "SamplerVoice.h"

#include "GainHelper.h"
#include "PitchHelper.h"

static constexpr float kTestLowpassFreq = 4500.f;
static constexpr float kTestLowpassQ = 1.f;

void SamplerVoice::init(double sampleRate) {
	playingSampleRate = sampleRate;
	player.init(sampleRate);
	configureFilters();
}

void SamplerVoice::configureFilters() {
	const float sampleRate = static_cast<float>(playingSampleRate);
	for(size_t channel = 0; channel < kMaxFilterChannels; channel++) {
		filters[channel].init(sampleRate);
		filters[channel].setLowpass(kTestLowpassFreq, kTestLowpassQ);
		filters[channel].reset();
	}
}

void SamplerVoice::resetFilters() {
	for(size_t channel = 0; channel < filterCount; channel++) {
		filters[channel].reset();
	}
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

	filterCount = slot.sample->getChannelCount();
	if(filterCount > kMaxFilterChannels) {
		filterCount = kMaxFilterChannels;
	}

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

	resetFilters();
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

void SamplerVoice::mixToBus(float* mixBus, size_t mixChannelCount) {

	/*for(size_t channel = 0; channel < kMaxFilterChannels; channel++) {
		filters[channel].setLowpass(kTestLowpassFreq, kTestLowpassQ);
	}*/

	if(filterCount <= 1) {
		const float filtered = filters[0].process(dry[0]) * gain;
		for(size_t channel = 0; channel < mixChannelCount; channel++) {
			mixBus[channel] += filtered;
		}
		return;
	}

	for(size_t channel = 0; channel < mixChannelCount && channel < filterCount; channel++) {
		mixBus[channel] += filters[channel].process(dry[channel]) * gain;
	}
}

void SamplerVoice::nextSamples(float* mixBus, size_t mixChannelCount) {
	dry[0] = 0.f;
	dry[1] = 0.f;
	player.nextSamples(dry, mixChannelCount);
	mixToBus(mixBus, mixChannelCount);
}
