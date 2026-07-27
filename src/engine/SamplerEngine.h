#pragma once

#include "MixBusArray.h"
#include "MidiInputDelegate.h"
#include "Program.h"
#include "SamplePlayerPool.h"
#include "VoiceAllocator.h"

class SamplerEngine : public MidiInputDelegate
{
public:
	void init(Program* program, double sampleRate, size_t playerCount);
	void onNoteOn(int note, int velocity) override;
	void onNoteOff(int note) override;
	void onControlChange(int controller, int value) override;
	void nextSamples(float* buf, size_t bufSize);

	size_t getPlayerCount() const { return playerPool.getCount(); }

private:
	void triggerSlot(const Program::Slot& slot, int velocity);

	Program* program = nullptr;
	SamplePlayerPool playerPool;
	VoiceAllocator voiceAllocator;
	MixBusArray mixBuses;
};
