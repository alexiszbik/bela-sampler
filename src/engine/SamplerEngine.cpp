#include "SamplerEngine.h"

#include "SamplerLog.h"

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
    if(channel != kSamplerChannel) {
        return;
    }

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
    if(channel != kSamplerChannel) {
        return;
    }

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
    if(channel != kSamplerChannel) {
        return;
    }

	SAMPLER_LOG("Control change: ctrl %d value %d\n", controller, value);
    
	const float ratioValue = static_cast<float>(value) / 127.f;

	for(const CCMap& map : ccMaps) {
		if(map.control != controller) {
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
    if(channel != kSamplerChannel) {
        return;
    }

	if(programBank == nullptr) {
		return;
	}

	if(!programBank->selectProgram(pgm)) {
		SAMPLER_LOG("Program change: unmapped PC %d\n", pgm);
		return;
	}

	const Program* program = programBank->getActiveProgram();
	SAMPLER_LOG("Program change: PC %d (%zu slots)\n",
		pgm,
		program != nullptr ? program->getSlotCount() : 0);
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	mixBuses.clearBusSums();
	playerPool.nextSamples(mixBuses);
	mixBuses.processAll(buf, bufSize);
}
