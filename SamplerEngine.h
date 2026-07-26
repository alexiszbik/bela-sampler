#pragma once

#include "MixBusArray.h"
#include "Program.h"
#include "SamplePlayerPool.h"
#include "VoiceAllocator.h"

class SamplerEngine
{
public:
	void init(Program* program, double sampleRate, size_t playerCount);
	void onNoteOn(int note, int velocity);
	void onNoteOff(int note);
	void onControlChange(int controller, int value);
	void nextSamples(float* buf, size_t bufSize);

	size_t getPlayerCount() const { return playerPool.getCount(); }

private:
	void triggerSlot(const Program::Slot& slot, int velocity);

	Program* program = nullptr;
	SamplePlayerPool playerPool;
	VoiceAllocator voiceAllocator;
	MixBusArray mixBuses;
};
