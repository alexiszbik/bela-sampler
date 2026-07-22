#pragma once

#include "SamplePlayer.h"

#include <vector>

class SamplePlayerPool
{
public:
	void init(double sampleRate, size_t count);
	void play(const Sample* sample);
	void nextSamples(float* buf, size_t bufSize);

	size_t getCount() const { return players.size(); }

private:
	SamplePlayer* findIdlePlayer();

	std::vector<SamplePlayer> players;
};
