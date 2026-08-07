#pragma once

#include "MixBusArray.h"
#include "MidiInputDelegate.h"
#include "Program.h"
#include "SamplePlayerPool.h"
#include "VoiceAllocator.h"

#include <array>
#include <cstdint>
#include <vector>

struct CCMap {
	int channel = 0;
	int control = 0;
	int busIndex = 0;
	ParameterIndex parameterIndex = LowPassCutoff;
};

class SamplerEngine : public MidiInputDelegate
{
public:
	void init(Program* program, double sampleRate, size_t playerCount);
	void onNoteOn(int note, int velocity, int channel) override;
	void onNoteOff(int note, int channel) override;
	void onControlChange(int controller, int value, int channel) override;
	void onPgmChange(int pgm, int channel) override;
	void nextSamples(float* buf, size_t bufSize);

	size_t getPlayerCount() const { return playerPool.getCount(); }

private:
	void triggerSlot(const Program::Slot& slot, int velocity);

	Program* program = nullptr;
	SamplePlayerPool playerPool;
	VoiceAllocator voiceAllocator;
	MixBusArray mixBuses;

	std::vector<CCMap> ccMaps = {
		CCMap{0, 20, 1, LowPassCutoff},
		CCMap{0, 21, 1, HiPassCutoff},
		CCMap{0, 22, 1, Volume},
		CCMap{0, 23, 1, DelayLevel},
		CCMap{0, 24, 1, DelayTime},
		CCMap{0, 25, 1, DelayFeedback},
		CCMap{0, 16, 1, ReverbSend},
	};
};
