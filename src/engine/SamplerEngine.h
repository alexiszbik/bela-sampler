#pragma once

#include "MixBusArray.h"
#include "MixBusNames.h"
#include "MidiInputDelegate.h"
#include "ProgramBank.h"
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
	int control = 0;
	std::vector<MapDest> destinations;
};

constexpr uint8_t kSamplerChannel = 3;

class SamplerEngine : public MidiInputDelegate
{
public:
	void init(ProgramBank* programBank, double sampleRate, size_t playerCount);
	void onNoteOn(int note, int velocity, int channel) override;
	void onNoteOff(int note, int channel) override;
	void onControlChange(int controller, int value, int channel) override;
	void onPgmChange(int pgm, int channel) override;
	void nextSamples(float* buf, size_t bufSize);

	size_t getPlayerCount() const { return playerPool.getCount(); }

private:
	void triggerSlot(const Program::Slot& slot, int velocity);

	ProgramBank* programBank = nullptr;
	SamplePlayerPool playerPool;
	VoiceAllocator voiceAllocator;
	MixBusArray mixBuses;

	std::vector<CCMap> ccMaps = {
		//============== SAMPLES

		CCMap{38, {{kBusSample, Volume}}},

		CCMap{52, {{kBusSample, Mute}}},

		CCMap{13, {{kBusSample, LowPassCutoff}}},
		CCMap{12, {{kBusSample, HiPassCutoff}}},

		CCMap{8, {{kBusSample, RepeatState}}},
		CCMap{10, {{kBusSample, RepeatRate}}},
		
		CCMap{11, {{kBusSample, BitCrushRate}}},

		CCMap{14, {{kBusSample, FlangerSpeed}}},
		CCMap{15, {{kBusSample, FlangerLevel}}},

		CCMap{41, {{kBusSample, DelayLevel}}},
		CCMap{40, {{kBusSample, DelayTime}}},
		CCMap{39, {{kBusSample, DelayFeedback}}},

		// ============= DRUMS

		CCMap{37, {{kBusKick, Volume}}},
		CCMap{36, {{kBusSnare, Volume}}},
		CCMap{35, {{kBusToms, Volume}}},
		CCMap{34, {{kBusHats, Volume}}},

		CCMap{53, {{kBusKick, Mute}}},
		CCMap{54, {{kBusSnare, Mute}}},
		CCMap{55, {{kBusToms, Mute}}},
		CCMap{56, {{kBusHats, Mute}}},

		CCMap{18, {{kBusKick, LowPassCutoff}, {kBusSnare, LowPassCutoff}, {kBusToms, LowPassCutoff}, {kBusHats, LowPassCutoff}}},
		CCMap{19, {{kBusKick, HiPassCutoff}, {kBusSnare, HiPassCutoff}, {kBusToms, HiPassCutoff}, {kBusHats, HiPassCutoff}}},

		CCMap{9, {{kBusKick, RepeatState}, {kBusSnare, RepeatState}, {kBusToms, RepeatState}, {kBusHats, RepeatState}}},
		CCMap{21, {{kBusKick, RepeatRate}, {kBusSnare, RepeatRate}, {kBusToms, RepeatRate}, {kBusHats, RepeatRate}}},
		
		CCMap{20, {{kBusSnare, BitCrushRate}, {kBusToms, BitCrushRate}, {kBusHats, BitCrushRate}}},
		CCMap{30, {{kBusHats, ReverbSend}}},
		CCMap{42, {{kBusSnare, ReverbSend}}},

		CCMap{17, {{kBusHats, FlangerSpeed}}},
		CCMap{16, {{kBusHats, FlangerLevel}}},

		CCMap{31, {{kBusToms, DelayLevel}}},
		CCMap{32, {{kBusToms, DelayTime}}},
		CCMap{33, {{kBusToms, DelayFeedback}}},

	};
};
