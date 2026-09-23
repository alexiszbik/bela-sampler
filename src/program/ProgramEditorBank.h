#pragma once

#include "ProgramJson.h"
#include "ProgramMapJson.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ProgramEditorBank
{
public:
	bool loadFromMap(const std::string& programFolder, const ProgramMap& programMap);
	void reloadFromDisk();

	std::vector<ProgramSlotDesc>& slotsForFile(const std::string& file);
	const std::vector<ProgramSlotDesc>& slotsForFile(const std::string& file) const;

	std::string filepathForFile(const std::string& file) const;
	std::string fileForPc(int pc) const;

	void markDirty(const std::string& file);
	void clearDirty(const std::string& file);
	bool isDirty(const std::string& file) const;
	bool hasAnyDirty() const;
	const std::unordered_set<std::string>& dirtyFiles() const { return dirty; }

	void invalidateMaterializedCache();

	std::vector<ProgramSlotDesc> layersAtRefSource(int refPc, int refMidiNote) const;
	std::vector<ProgramSlotDesc> materializedLayersForFile(const std::string& file) const;

	const ProgramMap& getProgramMap() const { return map; }

private:
	struct PendingSlotRef {
		int localMidiNote = 0;
		int refPc = -1;
		int refMidiNote = 0;
	};

	struct ProgramMaterialization {
		std::vector<ProgramSlotDesc> layers;
		std::vector<PendingSlotRef> pending;
	};

	static constexpr size_t kMaxRefResolvePasses = 8;

	void buildMaterializedCache() const;
	static void appendNonRefLayer(const ProgramSlotDesc& desc, std::vector<ProgramSlotDesc>& out);
	static void cloneLayerForLocalNote(const ProgramSlotDesc& source, int localMidiNote, std::vector<ProgramSlotDesc>& out);
	static bool resolvePendingOnce(ProgramMaterialization& host,
		const std::unordered_map<int, ProgramMaterialization>& byPc);

	std::string folder;
	ProgramMap map;
	std::unordered_map<std::string, std::vector<ProgramSlotDesc>> programsByFile;
	std::unordered_map<int, std::string> pcToFile;
	mutable std::unordered_map<int, std::vector<ProgramSlotDesc>> materializedByPc;
	mutable bool materializedCacheValid = false;
	std::unordered_set<std::string> dirty;
};
