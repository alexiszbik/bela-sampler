#pragma once

#include "SamplePlayer.h"

#include <vector>

class SamplePlayerPool
{
public:
	void init(double sampleRate, size_t count);
	void playOn(SamplePlayer* player, const Sample* sample);
	void nextSamples(float* buf, size_t bufSize);

	SamplePlayer* getPlayer(size_t playerIndex);
	size_t getCount() const { return players.size(); }

private:
	std::vector<SamplePlayer> players;
};
