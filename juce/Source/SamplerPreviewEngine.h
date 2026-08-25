#pragma once

#include "Program.h"
#include "ProgramJson.h"
#include "Sample.h"
#include "SamplerVoice.h"

#include <juce_core/juce_core.h>

#include <string>

class SamplerPreviewEngine
{
public:
	void prepare(double sampleRate, int blockSize);
	void playSlot(const ProgramSlotDesc& slotDesc, const juce::File& sampleFile);
	void stop();
	void mixInto(float* const* output, int numChannels, int numSamples);

private:
	bool ensureSampleLoaded(const juce::File& sampleFile, const std::string& relativePath);

	Sample sample;
	std::string loadedSamplePath;
	SamplerVoice voice;
	double sampleRate = 44100.0;
	int blockSize = 512;
};
