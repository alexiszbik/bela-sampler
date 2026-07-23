#include "SamplerEngine.h"

void SamplerEngine::init(Program* inProgram, double sampleRate, size_t playerCount) {
	program = inProgram;
	playerPool.init(sampleRate, playerCount);
	monoPlayerBySlot.clear();

	if(program != nullptr) {
		monoPlayerBySlot.assign(program->getSlotCount(), nullptr);
	}
}

bool SamplerEngine::isPlayerReservedForMono(const SamplePlayer* player) const {
	if(player == nullptr) {
		return false;
	}

	for(const SamplePlayer* monoPlayer : monoPlayerBySlot) {
		if(monoPlayer == player && player->getIsPlaying()) {
			return true;
		}
	}

	return false;
}

SamplePlayer* SamplerEngine::findFreePlayer() {
	for(size_t i = 0; i < playerPool.getCount(); i++) {
		SamplePlayer* player = playerPool.getPlayer(i);
		if(isPlayerReservedForMono(player)) {
			continue;
		}

		if(!player->getIsPlaying()) {
			return player;
		}
	}

	return nullptr;
}

void SamplerEngine::playPoly(const Program::Slot& slot) {
	SamplePlayer* player = findFreePlayer();
	if(player == nullptr) {
		return;
	}

	playerPool.playOn(player, slot.sample);
}

void SamplerEngine::playMono(const Program::Slot& slot) {
	if(slot.id >= monoPlayerBySlot.size()) {
		return;
	}

	SamplePlayer*& player = monoPlayerBySlot[slot.id];
	if(player == nullptr) {
		player = findFreePlayer();
		if(player == nullptr) {
			return;
		}
	}

	playerPool.playOn(player, slot.sample);
}

void SamplerEngine::onNoteOn(int note, int velocity) {
	if(program == nullptr || velocity <= 0) {
		return;
	}

	const Program::Slot* slot = program->getSlotForNote(note);
	if(slot == nullptr) {
		return;
	}

	if(slot->mode == Program::SlotMode::Mono) {
		playMono(*slot);
	} else {
		playPoly(*slot);
	}
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	playerPool.nextSamples(buf, bufSize);
}
