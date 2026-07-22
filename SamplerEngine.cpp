#include "SamplerEngine.h"

void SamplerEngine::init(Program* inProgram, SamplePlayerPool* pool) {
	program = inProgram;
	playerPool = pool;
}

void SamplerEngine::onNoteOn(int note, int velocity) {
	if(program == nullptr || playerPool == nullptr || velocity <= 0) {
		return;
	}

	const Sample* sample = program->getSampleForNote(note);
	if(sample == nullptr) {
		return;
	}

	playerPool->play(sample);
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	if(playerPool == nullptr) {
		return;
	}

	playerPool->nextSamples(buf, bufSize);
}
