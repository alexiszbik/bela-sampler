#pragma once

#include "JuceMidiBridge.h"
#include "MixBusArray.h"
#include "ProgramBank.h"
#include "SamplePreviewPlayer.h"
#include "SamplerBootstrap.h"
#include "SamplerEngine.h"
#include "SamplerMidiCollector.h"
#include "Sample.h"

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include <memory>
#include <vector>

class SamplerAudioEngine : public juce::AudioIODeviceCallback
{
public:
	static constexpr double kSampleRate = 44100.0;
	static constexpr int kBlockSize = 512;

	SamplerAudioEngine();
	~SamplerAudioEngine() override;

	bool initialise();
	void shutdown();
	bool reload();

	juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }
	const std::vector<Sample>& getSamples() const { return samples; }
	const ProgramBank& getProgramBank() const { return programBank; }
	SamplerEngine& getEngine() { return engine; }
	ProgramBank& getProgramBank() { return programBank; }
	SamplePreviewPlayer& getPreviewPlayer() { return previewPlayer; }

	void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
		int numInputChannels,
		float* const* outputChannelData,
		int numOutputChannels,
		int numSamples,
		const juce::AudioIODeviceCallbackContext& context) override;

	void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
	void audioDeviceStopped() override;

private:
	juce::AudioDeviceManager deviceManager;
	std::unique_ptr<juce::MidiInput> midiInput;
	std::unique_ptr<SamplerMidiCollector> midiCollector;

	std::vector<Sample> samples;
	ProgramBank programBank;
	SamplerEngine engine;
	JuceMidiBridge midiBridge;
	SamplePreviewPlayer previewPlayer;

	juce::MidiBuffer pendingMidi;
};
