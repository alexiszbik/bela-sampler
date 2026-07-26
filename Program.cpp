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

		if(sampleName.size() <= path.size()
			&& path.compare(path.size() - sampleName.size(), sampleName.size(), sampleName) == 0) {
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

Program::SlotPlayMode toProgramSlotPlayMode(ProgramSlotPlayMode playMode) {
	return playMode == ProgramSlotPlayMode::Granular
		? Program::SlotPlayMode::Granular
		: Program::SlotPlayMode::Normal;
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

void Program::addSlot(const ProgramSlotDesc& desc, const Sample* sample) {
	slots.push_back({
		slots.size(),
		desc.midiNote,
		sample,
		toProgramSlotMode(desc.mode),
		desc.muteGroup,
		desc.pitchSemitones,
		toProgramSlotPlayMode(desc.playMode),
		desc.granularSpeed,
		desc.reversed,
		desc.volumeDb,
		desc.bus
	});
}

bool Program::loadFromFile(const std::string& filepath, const std::vector<Sample>& samples) {
	slots.clear();

	std::vector<ProgramSlotDesc> slotDescs;
	ProgramJson programJson;
	if(!programJson.parseFile(filepath, slotDescs)) {
		return false;
	}

	for(const ProgramSlotDesc& slotDesc : slotDescs) {
		const char* groupName = muteGroupName(slotDesc.muteGroup);

		if(slotDesc.sample.empty()) {
			if(slotDesc.muteGroup == MuteGroup::None) {
				rt_printf("Program: slot note=%d missing sample\n", slotDesc.midiNote);
				continue;
			}

			addSlot(slotDesc, nullptr);
			rt_printf("Program slot: id=%zu note=%d mute-only muteGroup=%s bus=%d\n",
				slots.back().id,
				slotDesc.midiNote,
				groupName,
				slotDesc.bus);
			continue;
		}

		const Sample* sample = findSampleByName(samples, slotDesc.sample);
		if(sample == nullptr) {
			rt_printf("Program: sample not found: %s\n", slotDesc.sample.c_str());
			continue;
		}

		addSlot(slotDesc, sample);

		rt_printf("Program slot: id=%zu note=%d sample=%s mode=%s bus=%d\n",
			slots.back().id,
			slotDesc.midiNote,
			slotDesc.sample.c_str(),
			slotModeName(slots.back().mode),
			slotDesc.bus);
	}

	if(slots.empty()) {
		rt_printf("Program: no valid slots loaded from %s\n", filepath.c_str());
		return false;
	}

	return true;
}
