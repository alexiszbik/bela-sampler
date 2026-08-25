#include "SamplerPreviewEngine.h"

#include "SamplerLog.h"

namespace {
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

Program::SlotPlayMode toProgramSlotPlayMode(ProgramSlotPlayMode playMode) {
	return playMode == ProgramSlotPlayMode::Granular
		? Program::SlotPlayMode::Granular
		: Program::SlotPlayMode::Normal;
}

Program::Slot makePreviewSlot(const ProgramSlotDesc& desc, const Sample* samplePtr) {
	return {
		0,
		desc.midiNote,
		samplePtr,
		toProgramSlotMode(desc.mode),
		desc.muteGroup,
		desc.pitchSemitones,
		toProgramSlotPlayMode(desc.playMode),
		desc.granularSpeed,
		desc.reversed,
		desc.volumeDb,
		desc.bus
	};
}
}

void SamplerPreviewEngine::prepare(double inSampleRate, int inBlockSize) {
	sampleRate = inSampleRate;
	blockSize = inBlockSize;
	voice.init(sampleRate);
	(void)blockSize;
}

void SamplerPreviewEngine::stop() {
	voice.stop();
}

bool SamplerPreviewEngine::ensureSampleLoaded(const juce::File& sampleFile,
	const std::string& relativePath) {
	if(!sampleFile.existsAsFile()) {
		SAMPLER_LOG("Preview: file not found: %s\n", sampleFile.getFullPathName().toRawUTF8());
		return false;
	}

	if(loadedSamplePath == relativePath) {
		return true;
	}

	if(!sample.load(sampleFile.getFullPathName().toStdString(), relativePath)) {
		SAMPLER_LOG("Preview: could not load: %s\n", sampleFile.getFullPathName().toRawUTF8());
		return false;
	}

	loadedSamplePath = relativePath;
	return true;
}

void SamplerPreviewEngine::playSlot(const ProgramSlotDesc& slotDesc, const juce::File& sampleFile) {
	stop();

	if(slotDesc.sample.empty()) {
		return;
	}

	if(!ensureSampleLoaded(sampleFile, slotDesc.sample)) {
		return;
	}

	const Program::Slot slot = makePreviewSlot(slotDesc, &sample);
	voice.playOn(slot, 100);
}

void SamplerPreviewEngine::mixInto(float* const* output, int numChannels, int numSamples) {
	if(numChannels <= 0 || numSamples <= 0 || output == nullptr) {
		return;
	}

	for(int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex) {
		float stereo[2] = {0.f, 0.f};
		voice.nextSamples(stereo, 2, 2);

		if(output[0] != nullptr) {
			output[0][sampleIndex] += stereo[0];
		}

		if(numChannels > 1 && output[1] != nullptr) {
			output[1][sampleIndex] += stereo[1];
		}
	}
}
