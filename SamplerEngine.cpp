#include "SamplerEngine.h"

void SamplerEngine::init(Program* inProgram, double sampleRate, size_t playerCount) {
	program = inProgram;
	playerPool.init(sampleRate, playerCount);
	voiceAllocator.init(&playerPool);
}

void SamplerEngine::onNoteOn(int note, int velocity) {
	if(program == nullptr || velocity <= 0) {
		return;
	}

	const Program::Slot* slot = program->getSlotForNote(note);
	if(slot == nullptr) {
		return;
	}

	SamplePlayer* player = voiceAllocator.acquire(*slot);
	if(player == nullptr) {
		return;
	}

	playerPool.playOn(player, slot->sample, slot->mode == Program::SlotMode::Gate);
}

void SamplerEngine::onNoteOff(int note) {
	if(program == nullptr) {
		return;
	}

	const Program::Slot* slot = program->getSlotForNote(note);
	if(slot == nullptr || slot->mode != Program::SlotMode::Gate) {
		return;
	}

	voiceAllocator.releaseGate(*slot);
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	playerPool.nextSamples(buf, bufSize);
}
