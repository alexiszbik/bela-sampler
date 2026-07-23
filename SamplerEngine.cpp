#include "SamplerEngine.h"

void SamplerEngine::init(Program* inProgram, double sampleRate, size_t playerCount) {
	program = inProgram;
	playerPool.init(sampleRate, playerCount);
}

void SamplerEngine::onNoteOn(int note, int velocity) {
	if(program == nullptr || velocity <= 0) {
		return;
	}

	const Sample* sample = program->getSampleForNote(note);
	if(sample == nullptr) {
		return;
	}

	playerPool.play(sample);
}

void SamplerEngine::nextSamples(float* buf, size_t bufSize) {
	playerPool.nextSamples(buf, bufSize);
}
