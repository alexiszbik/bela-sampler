#include "SamplerVoice.h"

#include "GainHelper.h"
#include "PitchHelper.h"
#include "PanHelper.h"

void SamplerVoice::init(double sampleRate) {
	player.init(sampleRate);
}

void SamplerVoice::playOn(Program::Slot& slot, int velocity, size_t busChannelCount, QuadDispatch& dispatchState) {
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
	resolveDispatch(slot, busChannelCount, dispatchState);

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

void SamplerVoice::resolveDispatch(const Program::Slot& slot, size_t busChannelCount, QuadDispatch& dispatchState) {
	mixToAllChannels = false;
	mixLeftChannel = 0;
	mixRightChannel = busChannelCount > 1 ? 1 : 0;

	switch(slot.dispatch) {
		case SlotDispatch::Rear:
			mixLeftChannel = 2;
			mixRightChannel = 3;
			break;

		case SlotDispatch::All:
			mixToAllChannels = true;
			break;

		case SlotDispatch::Random:
		case SlotDispatch::Forward:
		case SlotDispatch::Backward: {
			const DispatchChannels channels = dispatchState.resolve(slot.dispatch, !isMonoSample, busChannelCount);
			mixLeftChannel = channels.left;
			mixRightChannel = channels.right;
			break;
		}

		case SlotDispatch::Front:
		default:
			break;
	}
}

void SamplerVoice::mixToStereoPair(float* sum, size_t leftChannel, size_t rightChannel, float left, float right, bool isMono) const {
	sum[leftChannel] += left;
	sum[rightChannel] += isMono ? left : right;
}

void SamplerVoice::mixToMonoChannel(float* sum, size_t channel, float sample) const {
	sum[channel] += sample;
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

	if(mixToAllChannels) {
		if(isMonoSample) {
			const float mono = dry[0] * gain;
			for(size_t channel = 0; channel < 4; ++channel) {
				sum[channel] += mono;
			}
		} else {
			mixToStereoPair(sum, 0, 1, left, right, false);
			mixToStereoPair(sum, 2, 3, left, right, false);
		}
		return;
	}

	const bool mixMonoToPair = isMonoSample && mixLeftChannel != mixRightChannel;
	if(mixMonoToPair) {
		mixToStereoPair(sum, mixLeftChannel, mixRightChannel, left, right, true);
	} else if(isMonoSample) {
		mixToMonoChannel(sum, mixLeftChannel, dry[0] * gain);
	} else {
		mixToStereoPair(sum, mixLeftChannel, mixRightChannel, left, right, false);
	}
}

void SamplerVoice::nextSamples(float* sum, size_t sumChannelCount) {
	dry[0] = 0.f;
	dry[1] = 0.f;

	player.nextSamples(dry, kMaxChannels);
	mixDryToSum(sum, sumChannelCount);
}
