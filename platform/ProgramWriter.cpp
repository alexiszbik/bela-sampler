#include "ProgramWriter.h"

#include <juce_core/juce_core.h>

namespace {
const char* modeToString(ProgramSlotMode mode) {
	switch(mode) {
		case ProgramSlotMode::Poly: return ProgramJson::kModePoly;
		case ProgramSlotMode::Mono: return ProgramJson::kModeMono;
		case ProgramSlotMode::Gate: return ProgramJson::kModeGate;
	}

	return ProgramJson::kModePoly;
}

const char* playModeToString(ProgramSlotPlayMode mode) {
	switch(mode) {
		case ProgramSlotPlayMode::Normal: return ProgramJson::kPlayModeNormal;
		case ProgramSlotPlayMode::Granular: return ProgramJson::kPlayModeGranular;
	}

	return ProgramJson::kPlayModeNormal;
}

const char* muteGroupToString(MuteGroup group) {
	switch(group) {
		case MuteGroup::None: return nullptr;
		case MuteGroup::A: return ProgramJson::kMuteGroupA;
		case MuteGroup::B: return ProgramJson::kMuteGroupB;
		case MuteGroup::C: return ProgramJson::kMuteGroupC;
		case MuteGroup::D: return ProgramJson::kMuteGroupD;
	}

	return nullptr;
}

std::unique_ptr<juce::DynamicObject> layerToObject(const ProgramSlotDesc& layer) {
	auto obj = std::make_unique<juce::DynamicObject>();

	if(!layer.sample.empty()) {
		obj->setProperty("sample", juce::String(layer.sample));
	}
	obj->setProperty("mode", juce::String(modeToString(layer.mode)));
	if(layer.volumeDb != 0.f) {
		obj->setProperty("volume", layer.volumeDb);
	}
	if(layer.pitchSemitones != 0.f) {
		obj->setProperty("pitch", layer.pitchSemitones);
	}
	if(layer.reversed) {
		obj->setProperty("reversed", 1);
	}
	if(layer.playMode == ProgramSlotPlayMode::Granular) {
		obj->setProperty("playmode", juce::String(playModeToString(layer.playMode)));
		obj->setProperty("granularSpeed", layer.granularSpeed);
	}
	if(layer.muteGroup != MuteGroup::None) {
		obj->setProperty("muteGroup", juce::String(muteGroupToString(layer.muteGroup)));
	}
	if(layer.bus != kBusMaster) {
		obj->setProperty("bus", juce::String(mixBusNickname(layer.bus)));
	}

	return obj;
}

juce::Array<juce::var> buildSlotsArray(const std::vector<ProgramSlotDesc>& slots) {
	juce::Array<juce::var> result;

	if(slots.empty()) {
		return result;
	}

	int currentNote = slots[0].midiNote;
	juce::Array<juce::var> currentLayers;
	currentLayers.add(layerToObject(slots[0]).get());

	for(size_t i = 1; i < slots.size(); ++i) {
		if(slots[i].midiNote == currentNote) {
			currentLayers.add(layerToObject(slots[i]).get());
		} else {
			auto slotObj = std::make_unique<juce::DynamicObject>();
			slotObj->setProperty("midiNote", currentNote);
			slotObj->setProperty("layers", currentLayers);
			result.add(slotObj.release());

			currentNote = slots[i].midiNote;
			currentLayers.clear();
			currentLayers.add(layerToObject(slots[i]).get());
		}
	}

	auto slotObj = std::make_unique<juce::DynamicObject>();
	slotObj->setProperty("midiNote", currentNote);
	slotObj->setProperty("layers", currentLayers);
	result.add(slotObj.release());

	return result;
}
}

bool ProgramWriter::writeProgram(const std::string& filepath, const std::vector<ProgramSlotDesc>& slots) {
	juce::DynamicObject root;
	root.setProperty("slots", buildSlotsArray(slots));

	const juce::String jsonText = juce::JSON::toString(&root, true);
	const juce::File file(filepath);

	if(!file.hasWriteAccess()) {
		return false;
	}

	return file.replaceWithText(jsonText);
}

bool ProgramWriter::writeProgramMap(const std::string& filepath, const ProgramMap& programMap) {
	juce::DynamicObject root;
	root.setProperty("defaultPc", programMap.defaultPc);

	juce::Array<juce::var> programs;
	for(const ProgramMapEntry& entry : programMap.entries) {
		auto obj = std::make_unique<juce::DynamicObject>();
		obj->setProperty("pc", entry.pc);
		obj->setProperty("file", juce::String(entry.file));
		programs.add(obj.release());
	}
	root.setProperty("programs", programs);

	const juce::String jsonText = juce::JSON::toString(&root, true);
	const juce::File file(filepath);

	if(!file.hasWriteAccess()) {
		return false;
	}

	return file.replaceWithText(jsonText);
}
