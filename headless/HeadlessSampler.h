#pragma once

#include "HeadlessConsoleUI.h"
#include "PreviewOutputLevels.h"
#include "MidiInputDelegate.h"
#include "MixBusArray.h"
#include "ProgramBank.h"
#include "RtMidiBridge.h"
#include "Sample.h"
#include "SamplerEngine.h"

#include <RtAudio.h>
#include <RtMidi.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

class HeadlessSampler
{
public:
	HeadlessSampler();
	~HeadlessSampler();

	bool initialise(const std::string& samplesFolder,
		const std::string& programFolder,
		const std::string& virtualPortName);
	void run();
	void stop();

private:
	static int audioCallback(void* outputBuffer, void* inputBuffer,
		unsigned int nFrames, double streamTime,
		RtAudioStreamStatus status, void* userData);
	static void midiCallback(double timestamp,
		std::vector<unsigned char>* message,
		void* userData);

	bool handleCommand(const std::string& line);
	bool reloadAll();
	bool startAudioStream();
	void stopAudioStream();

	std::vector<Sample> samples;
	ProgramBank programBank;
	SamplerEngine engine;
	RtMidiBridge midiBridge;

	std::unique_ptr<RtAudio> dac;
	std::unique_ptr<RtMidiIn> midiIn;

	std::string samplesFolder;
	std::string programFolder;
	std::string virtualPortName;
	static constexpr unsigned int kBlockSize = 512;
	std::array<PreviewOutputLevels, MixBusArray::kMasterChannelCount> channelLevels {};
	std::array<std::array<float, kBlockSize>, MixBusArray::kMasterChannelCount> meterChannels {};
	std::unique_ptr<HeadlessConsoleUI> consoleUI;
	bool running = false;
};
