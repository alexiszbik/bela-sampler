#include "SamplePlayerPool.h"

#include <Bela.h>

void SamplePlayerPool::init(double sampleRate, size_t count) {
	players.resize(count);
	for(SamplePlayer& player : players) {
		player.init(sampleRate);
	}
}

SamplePlayer* SamplePlayerPool::getPlayer(size_t playerIndex) {
	if(playerIndex >= players.size()) {
		return nullptr;
	}

	return &players[playerIndex];
}

void SamplePlayerPool::playOn(SamplePlayer* player, const Sample* sample) {
	if(sample == nullptr || player == nullptr) {
		return;
	}

	player->setSample(sample);
	player->trigger();

	const size_t playerIndex = static_cast<size_t>(player - &players[0]);
	rt_printf("Play sample %s on player %zu\n", sample->getName().c_str(), playerIndex);
}

void SamplePlayerPool::nextSamples(float* buf, size_t bufSize) {
	for(SamplePlayer& player : players) {
		player.nextSamples(buf, bufSize);
	}
}
