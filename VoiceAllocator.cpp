#include "VoiceAllocator.h"

void VoiceAllocator::init(SamplePlayerPool* pool) {
	playerPool = pool;
}

SamplePlayer* VoiceAllocator::findDedicatedPlayerForSlot(size_t slotId) const {
	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplePlayer& player : *playerPool) {
		const VoiceBinding& binding = player.getVoiceBinding();
		if(binding.isMonoOwner && binding.isBoundTo(slotId)) {
			return &player;
		}
	}

	return nullptr;
}

SamplePlayer* VoiceAllocator::findMuteGroupPlayer(MuteGroup group) const {
	if(playerPool == nullptr || group == MuteGroup::None) {
		return nullptr;
	}

	for(SamplePlayer& player : *playerPool) {
		if(player.getVoiceBinding().muteGroup == group) {
			return &player;
		}
	}

	return nullptr;
}

SamplePlayer* VoiceAllocator::findUnassignedPlayer() const {
	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplePlayer& player : *playerPool) {
		if(!player.getVoiceBinding().isAssigned()) {
			return &player;
		}
	}

	return nullptr;
}

SamplePlayer* VoiceAllocator::findFreePolyPlayer() const {
	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplePlayer& player : *playerPool) {
		if(player.getVoiceBinding().isAssigned()) {
			continue;
		}

		if(!player.getIsPlaying()) {
			return &player;
		}
	}

	return nullptr;
}

SamplePlayer* VoiceAllocator::acquireDedicatedPlayer(size_t slotId) {
	SamplePlayer* player = findDedicatedPlayerForSlot(slotId);
	if(player != nullptr) {
		return player;
	}

	SamplePlayer* candidate = findFreePolyPlayer();
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

SamplePlayer* VoiceAllocator::acquireGatePlayer(size_t slotId) {
	SamplePlayer* player = findFreePolyPlayer();
	if(player == nullptr) {
		return nullptr;
	}

	player->clearVoiceBinding();

	VoiceBinding binding;
	binding.slotId = slotId;
	player->setVoiceBinding(binding);
	return player;
}

SamplePlayer* VoiceAllocator::acquireMuteGroupPlayer(MuteGroup group) {
	SamplePlayer* player = findMuteGroupPlayer(group);
	if(player != nullptr) {
		return player;
	}

	SamplePlayer* candidate = findUnassignedPlayer();
	if(candidate == nullptr) {
		return nullptr;
	}

	VoiceBinding binding;
	binding.muteGroup = group;
	candidate->setVoiceBinding(binding);
	return candidate;
}

SamplePlayer* VoiceAllocator::acquire(const Program::Slot& slot) {
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

	for(SamplePlayer& player : *playerPool) {
		const VoiceBinding& binding = player.getVoiceBinding();
		if(!binding.isMonoOwner || player.getIsPlaying()) {
			continue;
		}

		player.clearVoiceBinding();
	}
}

void VoiceAllocator::stopMuteGroup(MuteGroup group) {
	if(playerPool == nullptr || group == MuteGroup::None) {
		return;
	}

	SamplePlayer* player = findMuteGroupPlayer(group);
	if(player != nullptr) {
		playerPool->stop(player);
	}
}

void VoiceAllocator::releaseGate(const Program::Slot& slot) {
	if(playerPool == nullptr) {
		return;
	}

	if(slot.muteGroup != MuteGroup::None) {
		SamplePlayer* player = findMuteGroupPlayer(slot.muteGroup);
		if(player != nullptr && player->getVoiceBinding().isActiveSlot(slot.id)) {
			playerPool->stop(player);
		}

		return;
	}

	for(SamplePlayer& player : *playerPool) {
		const VoiceBinding binding = player.getVoiceBinding();
		if(!binding.isBoundTo(slot.id)) {
			continue;
		}

		playerPool->stop(&player);

		if(!binding.isMonoOwner) {
			player.clearVoiceBinding();
		}
	}
}
