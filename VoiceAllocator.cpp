#include "VoiceAllocator.h"

void VoiceAllocator::init(SamplePlayerPool* pool) {
	playerPool = pool;
}

SamplerVoice* VoiceAllocator::findDedicatedPlayerForSlot(size_t slotId) const {
	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplerVoice& voice : *playerPool) {
		const VoiceBinding& binding = voice.getVoiceBinding();
		if(binding.isMonoOwner && binding.isBoundTo(slotId)) {
			return &voice;
		}
	}

	return nullptr;
}

SamplerVoice* VoiceAllocator::findMuteGroupPlayer(MuteGroup group) const {
	if(playerPool == nullptr || group == MuteGroup::None) {
		return nullptr;
	}

	for(SamplerVoice& voice : *playerPool) {
		if(voice.getVoiceBinding().muteGroup == group) {
			return &voice;
		}
	}

	return nullptr;
}

SamplerVoice* VoiceAllocator::findUnassignedPlayer() const {
	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplerVoice& voice : *playerPool) {
		if(!voice.getVoiceBinding().isAssigned()) {
			return &voice;
		}
	}

	return nullptr;
}

SamplerVoice* VoiceAllocator::findFreePolyPlayer() const {
	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplerVoice& voice : *playerPool) {
		if(voice.getVoiceBinding().isAssigned()) {
			continue;
		}

		if(!voice.getIsPlaying()) {
			return &voice;
		}
	}

	return nullptr;
}

SamplerVoice* VoiceAllocator::acquireDedicatedPlayer(size_t slotId) {
	SamplerVoice* voice = findDedicatedPlayerForSlot(slotId);
	if(voice != nullptr) {
		return voice;
	}

	SamplerVoice* candidate = findFreePolyPlayer();
	if(candidate == nullptr) {
		return nullptr;
	}

	candidate->clearVoiceBinding();

	VoiceBinding binding;
	binding.slotId = slotId;
	binding.isMonoOwner = true;
	candidate->setVoiceBinding(binding);
	return candidate;
}

SamplerVoice* VoiceAllocator::acquireGatePlayer(size_t slotId) {
	SamplerVoice* voice = findFreePolyPlayer();
	if(voice == nullptr) {
		return nullptr;
	}

	voice->clearVoiceBinding();

	VoiceBinding binding;
	binding.slotId = slotId;
	voice->setVoiceBinding(binding);
	return voice;
}

SamplerVoice* VoiceAllocator::acquireMuteGroupPlayer(MuteGroup group) {
	SamplerVoice* voice = findMuteGroupPlayer(group);
	if(voice != nullptr) {
		return voice;
	}

	SamplerVoice* candidate = findUnassignedPlayer();
	if(candidate == nullptr) {
		return nullptr;
	}

	VoiceBinding binding;
	binding.muteGroup = group;
	candidate->setVoiceBinding(binding);
	return candidate;
}

SamplerVoice* VoiceAllocator::acquire(const Program::Slot& slot) {
	releaseFinishedMonoVoices();

	if(slot.muteGroup != MuteGroup::None) {
		return acquireMuteGroupPlayer(slot.muteGroup);
	}

	if(slot.mode == Program::SlotMode::Poly) {
		return findFreePolyPlayer();
	}

	if(slot.mode == Program::SlotMode::Gate) {
		return acquireGatePlayer(slot.id);
	}

	return acquireDedicatedPlayer(slot.id);
}

void VoiceAllocator::releaseFinishedMonoVoices() {
	if(playerPool == nullptr) {
		return;
	}

	for(SamplerVoice& voice : *playerPool) {
		const VoiceBinding& binding = voice.getVoiceBinding();
		if(!binding.isMonoOwner || voice.getIsPlaying()) {
			continue;
		}

		voice.clearVoiceBinding();
	}
}

void VoiceAllocator::stopMuteGroup(MuteGroup group) {
	if(playerPool == nullptr || group == MuteGroup::None) {
		return;
	}

	SamplerVoice* voice = findMuteGroupPlayer(group);
	if(voice != nullptr) {
		playerPool->stop(voice);
	}
}

void VoiceAllocator::releaseGate(const Program::Slot& slot) {
	if(playerPool == nullptr) {
		return;
	}

	if(slot.muteGroup != MuteGroup::None) {
		SamplerVoice* voice = findMuteGroupPlayer(slot.muteGroup);
		if(voice != nullptr && voice->getVoiceBinding().isActiveSlot(slot.id)) {
			playerPool->stop(voice);
		}

		return;
	}

	for(SamplerVoice& voice : *playerPool) {
		const VoiceBinding binding = voice.getVoiceBinding();
		if(!binding.isBoundTo(slot.id)) {
			continue;
		}

		playerPool->stop(&voice);

		if(!binding.isMonoOwner) {
			voice.clearVoiceBinding();
		}
	}
}
