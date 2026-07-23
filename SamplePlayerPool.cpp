#include "SamplePlayerPool.h"

#include <Bela.h>

void SamplePlayerPool::init(double sampleRate, size_t count) {
	players.resize(count);
	for(SamplePlayer& player : players) {
		player.init(sampleRate);
	}
}

void SamplePlayerPool::playOn(SamplePlayer* player, const Sample* sample, bool loop) {
	if(sample == nullptr || player == nullptr) {
		return;
	}

	player->setSample(sample);
	player->setLoop(loop);
	player->trigger();

	const size_t playerIndex = static_cast<size_t>(player - &players[0]);
	rt_printf("Play sample %s on player %zu loop=%d\n",
		sample->getName().c_str(), playerIndex, loop ? 1 : 0);
}

void SamplePlayerPool::stop(SamplePlayer* player) {
	if(player == nullptr) {
		return;
	}

	player->stop();

	const size_t playerIndex = static_cast<size_t>(player - &players[0]);
	rt_printf("Stop player %zu\n", playerIndex);
}

void SamplePlayerPool::nextSamples(float* buf, size_t bufSize) {
	for(SamplePlayer& player : players) {
		player.nextSamples(buf, bufSize);
	}
}
