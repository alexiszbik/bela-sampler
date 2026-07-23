#include "Program.h"

#include "ProgramJson.h"

#include <Bela.h>

namespace {
std::string getBaseName(const std::string& path) {
	const size_t pos = path.find_last_of('/');
	if(pos == std::string::npos) {
		return path;
	}

	return path.substr(pos + 1);
}

const Sample* findSampleByName(const std::vector<Sample>& samples, const std::string& sampleName) {
	for(const Sample& sample : samples) {
		const std::string& path = sample.getName();
		if(path == sampleName || getBaseName(path) == sampleName) {
			return &sample;
		}
	}

	return nullptr;
}

Program::SlotMode toProgramSlotMode(ProgramSlotMode mode) {
	switch(mode) {
		case ProgramSlotMode::Mono:
			return Program::SlotMode::Mono;
		case ProgramSlotMode::Gate:
			return Program::SlotMode::Gate;
		default:
			return Program::SlotMode::Poly;
	}
}

const char* slotModeName(Program::SlotMode mode) {
	switch(mode) {
		case Program::SlotMode::Mono:
			return ProgramJson::kModeMono;
		case Program::SlotMode::Gate:
			return ProgramJson::kModeGate;
		default:
			return ProgramJson::kModePoly;
	}
}

const char* muteGroupName(MuteGroup muteGroup) {
	switch(muteGroup) {
		case MuteGroup::A:
			return ProgramJson::kMuteGroupA;
		case MuteGroup::B:
			return ProgramJson::kMuteGroupB;
		case MuteGroup::C:
			return ProgramJson::kMuteGroupC;
		case MuteGroup::D:
			return ProgramJson::kMuteGroupD;
		default:
			return nullptr;
	}
}
}

void Program::addSlot(int midiNote, const Sample* sample, SlotMode mode, MuteGroup muteGroup,
	float pitchSemitones) {
	slots.push_back({slots.size(), midiNote, sample, mode, muteGroup, pitchSemitones});
}

bool Program::loadFromFile(const std::string& filepath, const std::vector<Sample>& samples) {
	slots.clear();

	std::vector<ProgramSlotDesc> slotDescs;
	ProgramJson programJson;
	if(!programJson.parseFile(filepath, slotDescs)) {
		return false;
	}

	for(const ProgramSlotDesc& slotDesc : slotDescs) {
		const SlotMode mode = toProgramSlotMode(slotDesc.mode);
		const char* groupName = muteGroupName(slotDesc.muteGroup);

		if(slotDesc.sample.empty()) {
			if(slotDesc.muteGroup == MuteGroup::None) {
				rt_printf("Program: slot note=%d missing sample\n", slotDesc.midiNote);
				continue;
			}

			addSlot(slotDesc.midiNote, nullptr, mode, slotDesc.muteGroup, slotDesc.pitchSemitones);
			rt_printf("Program slot: id=%zu note=%d mute-only muteGroup=%s\n",
				slots.back().id,
				slotDesc.midiNote,
				groupName);
			continue;
		}

		const Sample* sample = findSampleByName(samples, slotDesc.sample);
		if(sample == nullptr) {
			rt_printf("Program: sample not found: %s\n", slotDesc.sample.c_str());
			continue;
		}

		addSlot(slotDesc.midiNote, sample, mode, slotDesc.muteGroup, slotDesc.pitchSemitones);

		if(groupName != nullptr) {
			rt_printf("Program slot: id=%zu note=%d sample=%s mode=%s muteGroup=%s pitch=%.2f\n",
				slots.back().id,
				slotDesc.midiNote,
				slotDesc.sample.c_str(),
				slotModeName(mode),
				groupName,
				slotDesc.pitchSemitones);
		} else if(slotDesc.pitchSemitones != 0.f) {
			rt_printf("Program slot: id=%zu note=%d sample=%s mode=%s pitch=%.2f\n",
				slots.back().id,
				slotDesc.midiNote,
				slotDesc.sample.c_str(),
				slotModeName(mode),
				slotDesc.pitchSemitones);
		} else {
			rt_printf("Program slot: id=%zu note=%d sample=%s mode=%s\n",
				slots.back().id,
				slotDesc.midiNote,
				slotDesc.sample.c_str(),
				slotModeName(mode));
		}
	}

	if(slots.empty()) {
		rt_printf("Program: no valid slots loaded from %s\n", filepath.c_str());
		return false;
	}

	return true;
}

const Program::Slot* Program::getSlotForNote(int note) const {
	for(const Slot& slot : slots) {
		if(slot.midiNote == note) {
			return &slot;
		}
	}

	return nullptr;
}
