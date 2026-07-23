#pragma once

#include "SamplePlayer.h"

#include <vector>

class SamplePlayerPool
{
public:
	void init(double sampleRate, size_t count);
	size_t findIdlePlayerIndex();
	void play(const Sample* sample);
	void playOn(size_t playerIndex, const Sample* sample);
	void nextSamples(float* buf, size_t bufSize);

	bool isPlaying(size_t playerIndex) const;
	size_t getCount() const { return players.size(); }

private:
	std::vector<SamplePlayer> players;
};
