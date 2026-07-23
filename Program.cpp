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
}

void Program::addSlot(int midiNote, const Sample* sample) {
	slots.push_back({midiNote, sample});
}

bool Program::loadFromFile(const std::string& filepath, const std::vector<Sample>& samples) {
	slots.clear();

	std::vector<ProgramSlotDesc> slotDescs;
	ProgramJson programJson;
	if(!programJson.parseFile(filepath, slotDescs)) {
		return false;
	}

	for(const ProgramSlotDesc& slotDesc : slotDescs) {
		const Sample* sample = findSampleByName(samples, slotDesc.sample);
		if(sample == nullptr) {
			rt_printf("Program: sample not found: %s\n", slotDesc.sample.c_str());
			continue;
		}

		addSlot(slotDesc.midiNote, sample);
		rt_printf("Program slot: note=%d sample=%s\n", slotDesc.midiNote, slotDesc.sample.c_str());
	}

	if(slots.empty()) {
		rt_printf("Program: no valid slots loaded from %s\n", filepath.c_str());
		return false;
	}

	return true;
}

const Sample* Program::getSampleForNote(int note) const {
	for(const Slot& slot : slots) {
		if(slot.midiNote == note) {
			return slot.sample;
		}
	}

	return nullptr;
}
