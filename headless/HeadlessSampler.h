#pragma once

#include "MidiInputDelegate.h"
#include "MixBusArray.h"
#include "ProgramBank.h"
#include "RtMidiBridge.h"
#include "Sample.h"
#include "SamplerEngine.h"

#include <RtAudio.h>
#include <RtMidi.h>

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

	void pollCommands(int timeoutMs);
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
	bool running = false;
};
