#pragma once

#include "MixBusArray.h"
#include "MixBusNames.h"
#include "MidiInputDelegate.h"
#include "Program.h"
#include "SamplePlayerPool.h"
#include "VoiceAllocator.h"

#include <array>
#include <cstdint>
#include <vector>

struct MapDest{
	MixBusIndex busIndex = kBusMaster;
	ParameterIndex parameterIndex = LowPassCutoff;
};

struct CCMap {
	int channel = 0;
	int control = 0;
	std::vector<MapDest> destinations;
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

		CCMap{3, 37, {{kBusKick, Volume}}},
		CCMap{3, 36, {{kBusSnare, Volume}}},
		CCMap{3, 35, {{kBusToms, Volume}}},
		CCMap{3, 34, {{kBusHats, Volume}}},

		CCMap{3, 53, {{kBusKick, Mute}}},
		CCMap{3, 54, {{kBusSnare, Mute}}},
		CCMap{3, 55, {{kBusToms, Mute}}},
		CCMap{3, 56, {{kBusHats, Mute}}},

		CCMap{3, 18, {{kBusKick, LowPassCutoff}, {kBusSnare, LowPassCutoff}, {kBusToms, LowPassCutoff}, {kBusHats, LowPassCutoff}}},
		CCMap{3, 19, {{kBusKick, HiPassCutoff}, {kBusSnare, HiPassCutoff}, {kBusToms, HiPassCutoff}, {kBusHats, HiPassCutoff}}},

		CCMap{3, 9, {{kBusKick, RepeatState}, {kBusSnare, RepeatState}, {kBusToms, RepeatState}, {kBusHats, RepeatState}}},
		CCMap{3, 21, {{kBusKick, RepeatRate}, {kBusSnare, RepeatRate}, {kBusToms, RepeatRate}, {kBusHats, RepeatRate}}},
		
		CCMap{3, 20, {{kBusSnare, BitCrushRate}, {kBusToms, BitCrushRate}, {kBusHats, BitCrushRate}}},
		CCMap{3, 30, {{kBusHats, ReverbSend}}},
		CCMap{3, 42, {{kBusSnare, ReverbSend}}},

		CCMap{3, 17, {{kBusHats, FlangerSpeed}}},
		CCMap{3, 16, {{kBusHats, FlangerLevel}}},

		CCMap{3, 31, {{kBusToms, DelayLevel}}},
		CCMap{3, 32, {{kBusToms, DelayTime}}},
		CCMap{3, 33, {{kBusToms, DelayFeedback}}},
	};
};
