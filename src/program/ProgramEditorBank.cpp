#include "ProgramEditorBank.h"

#include "SamplerLog.h"

#include <utility>

void ProgramEditorBank::appendNonRefLayer(const ProgramSlotDesc& desc, std::vector<ProgramSlotDesc>& out) {
	if(desc.isSlotRef()) {
		return;
	}

	if(!desc.sample.empty() || desc.muteGroup != MuteGroup::None) {
		out.push_back(desc);
	}
}

void ProgramEditorBank::cloneLayerForLocalNote(const ProgramSlotDesc& source,
	int localMidiNote,
	std::vector<ProgramSlotDesc>& out) {
	ProgramSlotDesc cloned = source;
	cloned.midiNote = localMidiNote;
	cloned.refPc = -1;
	cloned.refMidiNote = 0;
	out.push_back(cloned);
}

bool ProgramEditorBank::resolvePendingOnce(ProgramMaterialization& host,
	const std::unordered_map<int, ProgramMaterialization>& byPc) {
	if(host.pending.empty()) {
		return false;
	}

	bool resolvedAny = false;
	std::vector<PendingSlotRef> remaining;

	for(const PendingSlotRef& pending : host.pending) {
		const auto targetEntry = byPc.find(pending.refPc);
		if(targetEntry == byPc.end()) {
			remaining.push_back(pending);
			continue;
		}

		bool foundLayer = false;
		for(const ProgramSlotDesc& targetLayer : targetEntry->second.layers) {
			if(targetLayer.midiNote != pending.refMidiNote) {
				continue;
			}

			foundLayer = true;
			cloneLayerForLocalNote(targetLayer, pending.localMidiNote, host.layers);
			resolvedAny = true;
		}

		if(!foundLayer) {
			remaining.push_back(pending);
		}
	}

	host.pending = std::move(remaining);
	return resolvedAny;
}

bool ProgramEditorBank::loadFromMap(const std::string& programFolder, const ProgramMap& programMap) {
	folder = programFolder;
	map = programMap;
	programsByFile.clear();
	pcToFile.clear();
	dirty.clear();
	invalidateMaterializedCache();

	ProgramJson parser;
	for(const ProgramMapEntry& entry : programMap.entries) {
		const std::string filepath = folder + "/" + entry.file;
		std::vector<ProgramSlotDesc> slots;
		if(!parser.parseFile(filepath, slots)) {
			SAMPLER_LOG("ProgramEditorBank: could not load %s\n", filepath.c_str());
			return false;
		}

		programsByFile.emplace(entry.file, std::move(slots));
		pcToFile[entry.pc] = entry.file;
	}

	return !programsByFile.empty();
}

void ProgramEditorBank::reloadFromDisk() {
	const ProgramMap savedMap = map;
	loadFromMap(folder, savedMap);
}

std::vector<ProgramSlotDesc>& ProgramEditorBank::slotsForFile(const std::string& file) {
	return programsByFile.at(file);
}

const std::vector<ProgramSlotDesc>& ProgramEditorBank::slotsForFile(const std::string& file) const {
	return programsByFile.at(file);
}

std::string ProgramEditorBank::filepathForFile(const std::string& file) const {
	return folder + "/" + file;
}

std::string ProgramEditorBank::fileForPc(int pc) const {
	const auto entry = pcToFile.find(pc);
	if(entry == pcToFile.end()) {
		return {};
	}

	return entry->second;
}

void ProgramEditorBank::markDirty(const std::string& file) {
	dirty.insert(file);
	invalidateMaterializedCache();
}

bool ProgramEditorBank::isDirty(const std::string& file) const {
	return dirty.find(file) != dirty.end();
}

bool ProgramEditorBank::hasAnyDirty() const {
	return !dirty.empty();
}

void ProgramEditorBank::clearDirty(const std::string& file) {
	dirty.erase(file);
}

void ProgramEditorBank::invalidateMaterializedCache() {
	materializedByPc.clear();
	materializedCacheValid = false;
}

void ProgramEditorBank::buildMaterializedCache() const {
	if(materializedCacheValid) {
		return;
	}

	std::unordered_map<int, ProgramMaterialization> byPc;
	byPc.reserve(pcToFile.size());

	for(const auto& pcFile : pcToFile) {
		ProgramMaterialization state;
		const std::vector<ProgramSlotDesc>& raw = programsByFile.at(pcFile.second);

		for(const ProgramSlotDesc& desc : raw) {
			if(desc.isSlotRef()) {
				state.pending.push_back({desc.midiNote, desc.refPc, desc.refMidiNote});
			} else {
				appendNonRefLayer(desc, state.layers);
			}
		}

		byPc.emplace(pcFile.first, std::move(state));
	}

	for(size_t pass = 0; pass < kMaxRefResolvePasses; ++pass) {
		bool resolvedAny = false;
		for(auto& pcState : byPc) {
			resolvedAny |= resolvePendingOnce(pcState.second, byPc);
		}

		if(!resolvedAny) {
			break;
		}
	}

	materializedByPc.clear();
	for(const auto& pcState : byPc) {
		materializedByPc[pcState.first] = pcState.second.layers;
	}

	materializedCacheValid = true;
}

std::vector<ProgramSlotDesc> ProgramEditorBank::materializedLayersForFile(const std::string& file) const {
	buildMaterializedCache();

	for(const auto& entry : pcToFile) {
		if(entry.second == file) {
			const auto materialized = materializedByPc.find(entry.first);
			if(materialized != materializedByPc.end()) {
				return materialized->second;
			}

			break;
		}
	}

	return {};
}

std::vector<ProgramSlotDesc> ProgramEditorBank::layersAtRefSource(int refPc, int refMidiNote) const {
	buildMaterializedCache();

	const auto materialized = materializedByPc.find(refPc);
	if(materialized == materializedByPc.end()) {
		return {};
	}

	std::vector<ProgramSlotDesc> matched;
	for(const ProgramSlotDesc& layer : materialized->second) {
		if(layer.midiNote == refMidiNote) {
			matched.push_back(layer);
		}
	}

	return matched;
}
