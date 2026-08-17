#include "PreviewAudioHost.h"

#include "SamplerLog.h"

namespace {
constexpr double kSampleRate = 44100.0;
constexpr int kBlockSize = 512;
}

PreviewAudioHost::PreviewAudioHost() = default;

PreviewAudioHost::~PreviewAudioHost() {
	shutdown();
}

bool PreviewAudioHost::initialise() {
	const juce::String audioError = deviceManager.initialiseWithDefaultDevices(0, 2);
	if(audioError.isNotEmpty()) {
		SAMPLER_LOG("Preview audio init error: %s\n", audioError.toRawUTF8());
		return false;
	}

	auto* device = deviceManager.getCurrentAudioDevice();
	if(device != nullptr) {
		device->close();
		juce::AudioDeviceManager::AudioDeviceSetup setup;
		deviceManager.getAudioDeviceSetup(setup);
		setup.sampleRate = kSampleRate;
		setup.bufferSize = kBlockSize;
		const juce::String setupError = deviceManager.setAudioDeviceSetup(setup, true);
		if(setupError.isNotEmpty()) {
			SAMPLER_LOG("Preview audio setup error: %s\n", setupError.toRawUTF8());
			return false;
		}
	}

	deviceManager.addAudioCallback(this);
	player.prepare(kSampleRate, kBlockSize);
	return true;
}

void PreviewAudioHost::shutdown() {
	deviceManager.removeAudioCallback(this);
	player.stop();
	deviceManager.closeAudioDevice();
}

void PreviewAudioHost::audioDeviceAboutToStart(juce::AudioIODevice* device) {
	if(device != nullptr) {
		player.prepare(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());
	}
}

void PreviewAudioHost::audioDeviceStopped() {
	player.stop();
}

void PreviewAudioHost::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
	int numInputChannels,
	float* const* outputChannelData,
	int numOutputChannels,
	int numSamples,
	const juce::AudioIODeviceCallbackContext& context) {
	(void)inputChannelData;
	(void)numInputChannels;
	(void)context;

	if(numOutputChannels > 0 && outputChannelData[0] != nullptr) {
		juce::FloatVectorOperations::clear(outputChannelData[0], numSamples);
	}
	if(numOutputChannels > 1 && outputChannelData[1] != nullptr) {
		juce::FloatVectorOperations::clear(outputChannelData[1], numSamples);
	}

	player.mixInto(outputChannelData, numOutputChannels, numSamples);
}
