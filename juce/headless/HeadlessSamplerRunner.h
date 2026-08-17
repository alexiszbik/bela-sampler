#pragma once

#include "HeadlessMidiBridge.h"
#include "HeadlessMidiCollector.h"
#include "MixBusArray.h"
#include "ProgramBank.h"
#include "SamplerEngine.h"
#include "Sample.h"

#include <juce_audio_devices/juce_audio_devices.h>

#include <memory>
#include <vector>

class HeadlessSamplerRunner : private juce::AudioIODeviceCallback
{
public:
	HeadlessSamplerRunner();
	~HeadlessSamplerRunner() override;

	bool initialise(const std::string& samplesFolder, const std::string& programFolder, const std::string& midiDeviceName);
	void shutdown();

private:
	void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
		int numInputChannels,
		float* const* outputChannelData,
		int numOutputChannels,
		int numSamples,
		const juce::AudioIODeviceCallbackContext& context) override;

	void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
	void audioDeviceStopped() override;

	static constexpr double kSampleRate = 44100.0;
	static constexpr int kBlockSize = 512;
	static constexpr size_t kNumPlayers = 32;

	juce::AudioDeviceManager deviceManager;
	std::unique_ptr<juce::MidiInput> midiInput;
	HeadlessMidiCollector midiCollector;
	HeadlessMidiBridge midiBridge;

	std::vector<Sample> samples;
	ProgramBank programBank;
	SamplerEngine engine;
};
