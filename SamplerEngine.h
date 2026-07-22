#pragma once

#include "Program.h"
#include "SamplePlayerPool.h"

class SamplerEngine
{
public:
	void init(Program* program, SamplePlayerPool* pool);
	void onNoteOn(int note, int velocity);
	void nextSamples(float* buf, size_t bufSize);

private:
	Program* program = nullptr;
	SamplePlayerPool* playerPool = nullptr;
};
