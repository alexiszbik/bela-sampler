#pragma once

#include "MidiInputDelegate.h"
#include "MixBusArray.h"
#include "ProgramBank.h"
#include "RtMidiBridge.h"
#include "Sample.h"
#include "SamplerEngine.h"

#include <RtAudio.h>
#include <RtMidi.h>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class HeadlessSampler
{
public:
	HeadlessSampler();
	~HeadlessSampler();

	bool initialise(const std::string& samplesFolder,
		const std::string& programFolder,
		const std::string& midiDeviceName);
	void run();
	void stop();

private:
	static int audioCallback(void* outputBuffer, void* inputBuffer,
		unsigned int nFrames, double streamTime,
		RtAudioStreamStatus status, void* userData);
	static void midiCallback(double timestamp,
		std::vector<unsigned char>* message,
		void* userData);

	void midiWatcherLoop();
	bool findMidiPort(unsigned int& outPort) const;
	void openMidiPort(unsigned int port);
	void closeMidiPort();

	std::vector<Sample> samples;
	ProgramBank programBank;
	SamplerEngine engine;
	RtMidiBridge midiBridge;

	std::unique_ptr<RtAudio> dac;
	std::unique_ptr<RtMidiIn> midiIn;

	std::string midiDeviceName;
	std::atomic<bool> midiPortOpen {false};
	std::atomic<bool> running {false};
	std::thread midiWatcherThread;
};
