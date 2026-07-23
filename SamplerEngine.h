#pragma once

#include "Program.h"
#include "SamplePlayerPool.h"

#include <vector>

class SamplerEngine
{
public:
	void init(Program* program, double sampleRate, size_t playerCount);
	void onNoteOn(int note, int velocity);
	void nextSamples(float* buf, size_t bufSize);

	size_t getPlayerCount() const { return playerPool.getCount(); }

private:
	void playMono(const Program::Slot& slot);
	void playPoly(const Program::Slot& slot);
	bool isPlayerReservedForMono(const SamplePlayer* player) const;
	SamplePlayer* findFreePlayer();

	Program* program = nullptr;
	SamplePlayerPool playerPool;
	std::vector<SamplePlayer*> monoPlayerBySlot;
};
