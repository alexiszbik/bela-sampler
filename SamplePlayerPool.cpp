#include "SamplePlayerPool.h"

void SamplePlayerPool::init(double sampleRate, size_t count) {
	players.resize(count);
	for(SamplePlayer& player : players) {
		player.init(sampleRate);
	}
}

SamplePlayer* SamplePlayerPool::findIdlePlayer() {
	for(SamplePlayer& player : players) {
		if(!player.getIsPlaying()) {
			return &player;
		}
	}

	return nullptr;
}

void SamplePlayerPool::play(const Sample* sample) {
	if(sample == nullptr) {
		return;
	}

	SamplePlayer* player = findIdlePlayer();
	if(player == nullptr) {
		return;
	}

	player->setSample(sample);
	player->trigger();
}

void SamplePlayerPool::nextSamples(float* buf, size_t bufSize) {
	for(SamplePlayer& player : players) {
		player.nextSamples(buf, bufSize);
	}
}
