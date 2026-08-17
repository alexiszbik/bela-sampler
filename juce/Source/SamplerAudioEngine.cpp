#include "SamplerAudioEngine.h"

#include "SamplerDesktopPaths.h"
#include "SamplerLog.h"
#include "SamplerMidiCollector.h"

namespace {
constexpr const char* kMidiInputDeviceName = "ProtoSeq Virtual";

const juce::MidiDeviceInfo* findMidiInputDevice(const juce::Array<juce::MidiDeviceInfo>& devices) {
	for(const juce::MidiDeviceInfo& device : devices) {
		if(device.name == kMidiInputDeviceName) {
			return &device;
		}
	}

	return nullptr;
}
}

SamplerAudioEngine::SamplerAudioEngine() = default;

SamplerAudioEngine::~SamplerAudioEngine() {
	shutdown();
}

bool SamplerAudioEngine::initialise() {
	const std::string samplesFolder = SamplerDesktopPaths::getSamplesFolder();
	const std::string programFolder = SamplerDesktopPaths::getProgramFolder();

	SAMPLER_LOG("Desktop samples folder: %s\n", samplesFolder.c_str());
	SAMPLER_LOG("Desktop program folder: %s\n", programFolder.c_str());

	if(!SamplerBootstrap::init(samples,
			programBank,
			engine,
			kSampleRate,
			SamplerBootstrap::kDefaultPlayerCount,
			samplesFolder.c_str(),
			programFolder.c_str())) {
		return false;
	}

	midiBridge.setDelegate(&engine);
	midiCollector = std::make_unique<SamplerMidiCollector>(pendingMidi);

	juce::String audioError = deviceManager.initialiseWithDefaultDevices(0, 2);
	if(audioError.isNotEmpty()) {
		SAMPLER_LOG("Audio init error: %s\n", audioError.toRawUTF8());
		return false;
	}

	auto* device = deviceManager.getCurrentAudioDevice();
	if(device != nullptr) {
		device->close();
		juce::AudioDeviceManager::AudioDeviceSetup setup;
		deviceManager.getAudioDeviceSetup(setup);
		setup.sampleRate = kSampleRate;
		setup.bufferSize = kBlockSize;
		audioError = deviceManager.setAudioDeviceSetup(setup, true);
		if(audioError.isNotEmpty()) {
			SAMPLER_LOG("Audio setup error: %s\n", audioError.toRawUTF8());
			return false;
		}
	}

	deviceManager.addAudioCallback(this);
	previewPlayer.prepare(kSampleRate, kBlockSize);

	const auto midiDevices = juce::MidiInput::getAvailableDevices();
	const juce::MidiDeviceInfo* midiDevice = findMidiInputDevice(midiDevices);
	if(midiDevice != nullptr) {
		midiInput = juce::MidiInput::openDevice(midiDevice->identifier, midiCollector.get());
		if(midiInput != nullptr) {
			midiInput->start();
			SAMPLER_LOG("MIDI input enabled on %s\n", midiDevice->name.toRawUTF8());
		}
	} else {
		SAMPLER_LOG("MIDI input device not found: %s\n", kMidiInputDeviceName);
		for(const juce::MidiDeviceInfo& device : midiDevices) {
			SAMPLER_LOG("  available: %s\n", device.name.toRawUTF8());
		}
	}

	return true;
}

bool SamplerAudioEngine::reload() {
	const std::string samplesFolder = SamplerDesktopPaths::getSamplesFolder();
	const std::string programFolder = SamplerDesktopPaths::getProgramFolder();

	samples.clear();
	programBank = {};
	engine = {};

	if(!SamplerBootstrap::init(samples,
			programBank,
			engine,
			kSampleRate,
			SamplerBootstrap::kDefaultPlayerCount,
			samplesFolder.c_str(),
			programFolder.c_str())) {
		return false;
	}

	midiBridge.setDelegate(&engine);
	SAMPLER_LOG("Engine reloaded\n");
	return true;
}

void SamplerAudioEngine::shutdown() {
	deviceManager.removeAudioCallback(this);
	previewPlayer.stop();

	if(midiInput != nullptr) {
		midiInput->stop();
		midiInput.reset();
	}

	midiCollector.reset();
	deviceManager.closeAudioDevice();
	samples.clear();
}

void SamplerAudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device) {
	if(device != nullptr) {
		previewPlayer.prepare(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());
	}
}

void SamplerAudioEngine::audioDeviceStopped() {
	previewPlayer.stop();
}

void SamplerAudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
	int numInputChannels,
	float* const* outputChannelData,
	int numOutputChannels,
	int numSamples,
	const juce::AudioIODeviceCallbackContext& context) {
	(void)inputChannelData;
	(void)numInputChannels;
	(void)context;

	juce::MidiBuffer midiToProcess;
	if(midiCollector != nullptr) {
		midiCollector->swapBuffer(midiToProcess);
	}
	midiBridge.processMidiBuffer(midiToProcess);

	for(int sample = 0; sample < numSamples; ++sample) {
		float mix[MixBusArray::kMasterChannelCount] = {};
		engine.nextSamples(mix, MixBusArray::kMasterChannelCount);
        
        float monoMix =mix[4] + + mix[5] + mix[6] + mix[7];

		if(numOutputChannels > 0 && outputChannelData[0] != nullptr) {
			outputChannelData[0][sample] = mix[0] + mix[2] + monoMix;
		}
		if(numOutputChannels > 1 && outputChannelData[1] != nullptr) {
            outputChannelData[1][sample] = mix[1] + mix[3] + monoMix;
		}
	}

	previewPlayer.mixInto(outputChannelData, numOutputChannels, numSamples);
}
