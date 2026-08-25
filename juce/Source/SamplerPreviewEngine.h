#pragma once

#include "Program.h"
#include "ProgramJson.h"
#include "Sample.h"
#include "SamplerVoice.h"

#include <juce_core/juce_core.h>

#include <functional>
#include <string>

class SamplerPreviewEngine
{
public:
	using SamplePreviewCallback = std::function<void(const Sample&,
		bool reversed,
		float volumeDb,
		const std::string& displayName)>;

	void prepare(double sampleRate, int blockSize);
	void playSlot(const ProgramSlotDesc& slotDesc, const juce::File& sampleFile);
	void stop();
	void mixInto(float* const* output, int numChannels, int numSamples);

	SamplePreviewCallback onSamplePreviewed;

private:
	bool ensureSampleLoaded(const juce::File& sampleFile, const std::string& relativePath);

	Sample sample;
	std::string loadedSamplePath;
	SamplerVoice voice;
	double sampleRate = 44100.0;
	int blockSize = 512;
};
