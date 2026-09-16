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

SamplerVoice* VoiceAllocator::findMuteGroupVoiceForSlot(const Program::Slot& slot) const {
	if(playerPool == nullptr || slot.muteGroup == MuteGroup::None) {
		return nullptr;
	}

	for(SamplerVoice& voice : *playerPool) {
		const VoiceBinding& binding = voice.getVoiceBinding();
		if(binding.muteGroup == slot.muteGroup
			&& binding.muteGroupNote == slot.midiNote
			&& binding.isActiveSlot(slot.id)) {
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

SamplerVoice* VoiceAllocator::acquireMuteGroupPlayer(const Program::Slot& slot) {
	stopMuteGroupExceptNote(slot.muteGroup, slot.midiNote);

	SamplerVoice* existingVoice = findMuteGroupVoiceForSlot(slot);
	if(existingVoice != nullptr) {
		return existingVoice;
	}

	SamplerVoice* candidate = findFreePolyPlayer();
	if(candidate == nullptr) {
		candidate = findUnassignedPlayer();
	}

	if(candidate == nullptr) {
		return nullptr;
	}

	candidate->clearVoiceBinding();

	VoiceBinding binding;
	binding.muteGroup = slot.muteGroup;
	binding.muteGroupNote = slot.midiNote;
	candidate->setVoiceBinding(binding);
	return candidate;
}

SamplerVoice* VoiceAllocator::acquire(const Program::Slot& slot) {
	releaseFinishedVoices();

	if(slot.muteGroup != MuteGroup::None) {
		return acquireMuteGroupPlayer(slot);
	}

	if(slot.mode == Program::SlotMode::Poly) {
		return findFreePolyPlayer();
	}

	if(slot.mode == Program::SlotMode::Gate) {
		return acquireGatePlayer(slot.id);
	}

	return acquireDedicatedPlayer(slot.id);
}

void VoiceAllocator::releaseFinishedVoices() {
	if(playerPool == nullptr) {
		return;
	}

	for(SamplerVoice& voice : *playerPool) {
		const VoiceBinding& binding = voice.getVoiceBinding();
		if(voice.getIsPlaying()) {
			continue;
		}

		if(binding.isMonoOwner || binding.muteGroup != MuteGroup::None) {
			voice.clearVoiceBinding();
		}
	}
}

void VoiceAllocator::stopMuteGroupExceptNote(MuteGroup group, int midiNote) {
	if(playerPool == nullptr || group == MuteGroup::None) {
		return;
	}

	for(SamplerVoice& voice : *playerPool) {
		const VoiceBinding& binding = voice.getVoiceBinding();
		if(binding.muteGroup != group || binding.muteGroupNote == midiNote) {
			continue;
		}

		playerPool->stop(&voice);
		voice.clearVoiceBinding();
	}
}

void VoiceAllocator::releaseGate(const Program::Slot& slot) {
	if(playerPool == nullptr) {
		return;
	}

	if(slot.muteGroup != MuteGroup::None) {
		for(SamplerVoice& voice : *playerPool) {
			if(voice.getVoiceBinding().isActiveSlot(slot.id)) {
				playerPool->stop(&voice);
				voice.clearVoiceBinding();
			}
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
