#include "SamplerEngine.h"

void SamplerEngine::init(Program* inProgram, double sampleRate, size_t playerCount) {
	program = inProgram;
	playerPool.init(sampleRate, playerCount);
	monoPlayerBySlotId.clear();

	if(program != nullptr) {
		monoPlayerBySlotId.assign(program->getSlotCount(), playerPool.getCount());
	}
}

bool SamplerEngine::isPlayerReservedForMono(size_t playerIndex) const {
	for(size_t slotId = 0; slotId < monoPlayerBySlotId.size(); slotId++) {
		if(monoPlayerBySlotId[slotId] != playerIndex || playerIndex >= playerPool.getCount()) {
			continue;
		}

		if(playerPool.isPlaying(playerIndex)) {
			return true;
		}
	}

	return false;
}

size_t SamplerEngine::findFreePlayerIndex() const {
	for(size_t i = 0; i < playerPool.getCount(); i++) {
		if(isPlayerReservedForMono(i)) {
			continue;
		}

		if(!playerPool.isPlaying(i)) {
			return i;
		}
	}

	return playerPool.getCount();
}

void SamplerEngine::playPoly(const Program::Slot& slot) {
	const size_t playerIndex = findFreePlayerIndex();
	if(playerIndex >= playerPool.getCount()) {
		return;
	}

	playerPool.playOn(playerIndex, slot.sample);
}

void SamplerEngine::playMono(const Program::Slot& slot) {
	if(slot.id >= monoPlayerBySlotId.size()) {
		return;
	}

	size_t playerIndex = monoPlayerBySlotId[slot.id];
	if(playerIndex >= playerPool.getCount()) {
		playerIndex = findFreePlayerIndex();
		if(playerIndex >= playerPool.getCount()) {
			return;
		}

		monoPlayerBySlotId[slot.id] = playerIndex;
	}

	playerPool.playOn(playerIndex, slot.sample);
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
