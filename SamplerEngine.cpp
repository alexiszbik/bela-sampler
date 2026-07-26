#include "SamplerEngine.h"

void SamplerEngine::init(Program* inProgram, double sampleRate, size_t playerCount) {
	program = inProgram;
	playerPool.init(sampleRate, playerCount);
	voiceAllocator.init(&playerPool);
	mixBuses.init(sampleRate);
}

void SamplerEngine::triggerSlot(const Program::Slot& slot, int velocity) {
	if(slot.isMuteOnly()) {
		voiceAllocator.stopMuteGroup(slot.muteGroup);
		return;
	}

	SamplerVoice* voice = voiceAllocator.acquire(slot);
	if(voice == nullptr) {
		return;
	}

	playerPool.playOn(voice, slot, velocity);
}

void SamplerEngine::onNoteOn(int note, int velocity) {
	if(program == nullptr || velocity <= 0) {
		return;
	}

	for(const Program::Slot& slot : program->getSlots()) {
		if(slot.midiNote != note) {
			continue;
		}

		triggerSlot(slot, velocity);
	}
}

void SamplerEngine::onNoteOff(int note) {
	if(program == nullptr) {
		return;
	}

	for(const Program::Slot& slot : program->getSlots()) {
		if(slot.midiNote != note || slot.mode != Program::SlotMode::Gate) {
			continue;
		}

		voiceAllocator.releaseGate(slot);
	}
}

void SamplerEngine::onControlChange(int controller, int value) {
	if(controller != MixBusArray::kBus1FilterCc) {
		return;
	}

	const float cutoffHz = MixBusArray::bus1CutoffFromCc(value);
	mixBuses.setBusLowpassCutoff(1, cutoffHz);
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	float busSums[MixBusArray::kBusCount][MixBusArray::kBusChannels] = {{0.f}};

	playerPool.nextSamples(busSums, MixBusArray::kBusCount, MixBusArray::kBusChannels);
	mixBuses.processBuses(busSums, MixBusArray::kBusChannels);
	mixBuses.writeToOutput(busSums, buf, bufSize);
}
