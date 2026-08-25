#pragma once

#include "SamplerPreviewEngine.h"

#include <juce_audio_devices/juce_audio_devices.h>

class PreviewAudioHost : private juce::AudioIODeviceCallback
{
public:
	PreviewAudioHost();
	~PreviewAudioHost() override;

	bool initialise();
	void shutdown();

	SamplerPreviewEngine& getPlayer() { return player; }

private:
	void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
		int numInputChannels,
		float* const* outputChannelData,
		int numOutputChannels,
		int numSamples,
		const juce::AudioIODeviceCallbackContext& context) override;

	void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
	void audioDeviceStopped() override;

	juce::AudioDeviceManager deviceManager;
	SamplerPreviewEngine player;
};
