#pragma once

#include "Program.h"
#include "SamplePlayerPool.h"

class SamplerEngine
{
public:
	void init(Program* program, double sampleRate, size_t playerCount);
	void onNoteOn(int note, int velocity);
	void nextSamples(float* buf, size_t bufSize);

	size_t getPlayerCount() const { return playerPool.getCount(); }

private:
	Program* program = nullptr;
	SamplePlayerPool playerPool;
};
