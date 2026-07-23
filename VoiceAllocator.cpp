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

SamplePlayer* VoiceAllocator::findFreePolyPlayer() const {
	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplePlayer& player : *playerPool) {
		if(player.getVoiceBinding().isMonoOwner) {
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

	if(playerPool == nullptr) {
		return nullptr;
	}

	for(SamplePlayer& candidate : *playerPool) {
		if(candidate.getVoiceBinding().isMonoOwner) {
			continue;
		}

		VoiceBinding binding;
		binding.slotId = slotId;
		binding.isMonoOwner = true;
		candidate.setVoiceBinding(binding);
		return &candidate;
	}

	return nullptr;
}

SamplePlayer* VoiceAllocator::acquire(const Program::Slot& slot) {
	if(slot.mode == Program::SlotMode::Poly) {
		return findFreePolyPlayer();
	}

	return acquireDedicatedPlayer(slot.id);
}

void VoiceAllocator::releaseGate(const Program::Slot& slot) {
	if(playerPool == nullptr) {
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
