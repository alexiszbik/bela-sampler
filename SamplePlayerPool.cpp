#include "SamplePlayerPool.h"

#include <Bela.h>

void SamplePlayerPool::init(double sampleRate, size_t count) {
	players.resize(count);
	for(SamplePlayer& player : players) {
		player.init(sampleRate);
	}
}

size_t SamplePlayerPool::findIdlePlayerIndex() {
	for(size_t i = 0; i < players.size(); i++) {
		if(!players[i].getIsPlaying()) {
			return i;
		}
	}

	return players.size();
}

bool SamplePlayerPool::isPlaying(size_t playerIndex) const {
	if(playerIndex >= players.size()) {
		return false;
	}

	return players[playerIndex].getIsPlaying();
}

void SamplePlayerPool::playOn(size_t playerIndex, const Sample* sample) {
	if(sample == nullptr || playerIndex >= players.size()) {
		return;
	}

	SamplePlayer& player = players[playerIndex];
	player.setSample(sample);
	player.trigger();

	rt_printf("Play sample %s on player %zu\n", sample->getName().c_str(), playerIndex);
}

void SamplePlayerPool::play(const Sample* sample) {
	if(sample == nullptr) {
		return;
	}

	const size_t playerIndex = findIdlePlayerIndex();
	if(playerIndex >= players.size()) {
		return;
	}

	playOn(playerIndex, sample);
}

void SamplePlayerPool::nextSamples(float* buf, size_t bufSize) {
	for(SamplePlayer& player : players) {
		player.nextSamples(buf, bufSize);
	}
}
