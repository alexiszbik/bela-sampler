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

void SamplerEngine::onNoteOn(int note, int velocity, int channel) {
	(void)channel;

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

void SamplerEngine::onNoteOff(int note, int channel) {
	(void)channel;

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

void SamplerEngine::onControlChange(int controller, int value, int channel) {
	const float ratioValue = static_cast<float>(value) / 127.f;

	for(const CCMap& map : ccMaps) {
		if(map.control != controller) {
			continue;
		}

		if(map.channel != channel) {
			continue;
		}

		mixBuses.setBusParameter(
			static_cast<size_t>(map.busIndex),
			map.parameterIndex,
			ratioValue);
	}
}

void SamplerEngine::onPgmChange(int pgm, int channel) {
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	mixBuses.clearBusSums();
	playerPool.nextSamples(mixBuses);
	mixBuses.processAll(buf, bufSize);
}
