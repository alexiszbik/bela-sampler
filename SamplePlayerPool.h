#pragma once

#include "Program.h"
#include "SamplePlayer.h"

#include <vector>

class SamplePlayerPool
{
public:
	void init(double sampleRate, size_t count);
	void playOn(SamplePlayer* player, const Program::Slot& slot, int velocity);
	void stop(SamplePlayer* player);
	void nextSamples(float* buf, size_t bufSize);

	size_t getCount() const { return players.size(); }

	using PlayerIterator = std::vector<SamplePlayer>::iterator;
	using PlayerConstIterator = std::vector<SamplePlayer>::const_iterator;

	PlayerIterator begin() { return players.begin(); }
	PlayerIterator end() { return players.end(); }
	PlayerConstIterator begin() const { return players.begin(); }
	PlayerConstIterator end() const { return players.end(); }

private:
	std::vector<SamplePlayer> players;
};
