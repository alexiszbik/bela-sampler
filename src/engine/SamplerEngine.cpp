#include "SamplerEngine.h"

#include <Bela.h>

void SamplerEngine::init(ProgramBank* inProgramBank, double sampleRate, size_t playerCount) {
	programBank = inProgramBank;
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

	Program* program = programBank != nullptr ? programBank->getActiveProgram() : nullptr;
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

	Program* program = programBank != nullptr ? programBank->getActiveProgram() : nullptr;
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

		for (auto& dest : map.destinations) {
			mixBuses.setBusParameter(
				dest.busIndex,
				dest.parameterIndex,
				ratioValue);
		}


	}
}

void SamplerEngine::onPgmChange(int pgm, int channel) {
	(void)channel;

	if(programBank == nullptr) {
		return;
	}

	if(!programBank->selectProgram(pgm)) {
		rt_printf("Program change: unmapped PC %d\n", pgm);
		return;
	}

	const Program* program = programBank->getActiveProgram();
	rt_printf("Program change: PC %d (%zu slots)\n",
		pgm,
		program != nullptr ? program->getSlotCount() : 0);
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	mixBuses.clearBusSums();
	playerPool.nextSamples(mixBuses);
	mixBuses.processAll(buf, bufSize);
}
