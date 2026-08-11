#include "ProgramBank.h"

#include "ProgramMapJson.h"

#include "SamplerLog.h"

#include <unordered_map>

namespace {
std::string joinPath(const std::string& folder, const std::string& file) {
	if(folder.empty()) {
		return file;
	}

	return folder + "/" + file;
}
}

bool ProgramBank::load(const std::string& programFolder, const std::vector<Sample>& samples) {
	programs.clear();
	pcToProgramIndex.clear();
	activeProgramIndex = 0;
	activePc = 0;

	ProgramMap programMap;
	ProgramMapJson programMapJson;
	const std::string mapPath = joinPath(programFolder, "program_map.json");
	if(!programMapJson.parseFile(mapPath, programMap)) {
		return false;
	}

	std::unordered_map<std::string, size_t> fileToProgramIndex;

	for(const ProgramMapEntry& entry : programMap.entries) {
		if(entry.file.empty()) {
			SAMPLER_LOG("ProgramBank: empty program file for PC %d\n", entry.pc);
			continue;
		}

		size_t programIndex = 0;
		const auto existing = fileToProgramIndex.find(entry.file);
		if(existing == fileToProgramIndex.end()) {
			programs.emplace_back();
			const std::string programPath = joinPath(programFolder, entry.file);
			if(!programs.back().loadFromFile(programPath, samples)) {
				SAMPLER_LOG("ProgramBank: failed to load %s\n", programPath.c_str());
				programs.pop_back();
				continue;
			}

			programIndex = programs.size() - 1;
			fileToProgramIndex.emplace(entry.file, programIndex);
			SAMPLER_LOG("ProgramBank: loaded %s (%zu slots)\n",
				entry.file.c_str(),
				programs.back().getSlotCount());
		} else {
			programIndex = existing->second;
		}

		pcToProgramIndex[entry.pc] = programIndex;
		SAMPLER_LOG("ProgramBank: PC %d -> %s\n", entry.pc, entry.file.c_str());
	}

	if(programs.empty() || pcToProgramIndex.empty()) {
		SAMPLER_LOG("ProgramBank: no programs loaded from %s\n", mapPath.c_str());
		return false;
	}

	const auto defaultEntry = pcToProgramIndex.find(programMap.defaultPc);
	if(defaultEntry == pcToProgramIndex.end()) {
		SAMPLER_LOG("ProgramBank: default PC %d not mapped, using first entry\n", programMap.defaultPc);
		activeProgramIndex = pcToProgramIndex.begin()->second;
		activePc = pcToProgramIndex.begin()->first;
	} else {
		activeProgramIndex = defaultEntry->second;
		activePc = programMap.defaultPc;
	}

	SAMPLER_LOG("ProgramBank: active PC %d (%zu slots)\n",
		activePc,
		programs[activeProgramIndex].getSlotCount());

	return true;
}

Program* ProgramBank::getActiveProgram() {
	if(programs.empty()) {
		return nullptr;
	}

	return &programs[activeProgramIndex];
}

const Program* ProgramBank::getActiveProgram() const {
	if(programs.empty()) {
		return nullptr;
	}

	return &programs[activeProgramIndex];
}

bool ProgramBank::selectProgram(int pc) {
	const auto entry = pcToProgramIndex.find(pc);
	if(entry == pcToProgramIndex.end()) {
		return false;
	}

	if(activePc == pc && entry->second == activeProgramIndex) {
		return true;
	}

	activePc = pc;
	activeProgramIndex = entry->second;
	return true;
}
