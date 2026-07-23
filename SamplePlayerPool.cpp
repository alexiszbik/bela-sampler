#include "SamplePlayerPool.h"

#include "PitchHelper.h"
#include "ProgramJson.h"

#include <Bela.h>

void SamplePlayerPool::init(double sampleRate, size_t count) {
	players.resize(count);
	for(SamplePlayer& player : players) {
		player.init(sampleRate);
	}
}

void SamplePlayerPool::playOn(SamplePlayer* player, const Program::Slot& slot, int velocity) {
	if(player == nullptr || slot.sample == nullptr) {
		return;
	}

	const bool loop = slot.mode == Program::SlotMode::Gate;
	const float pitchSpeed = semitonesToPlaybackSpeed(slot.pitchSemitones);
	const SamplePlayer::EPlayerMode playerMode = slot.playMode == Program::SlotPlayMode::Granular
		? SamplePlayer::Granular
		: SamplePlayer::Normal;
	const float gain = static_cast<float>(velocity) / 127.f;

	player->setSample(slot.sample);
	player->setLoop(loop);
	player->setReversed(slot.reversed);
	player->setPlayMode(playerMode);
	player->setGain(gain*gain);

	if(slot.playMode == Program::SlotPlayMode::Granular) {
		player->setGranularSpeed(slot.granularSpeed);
		player->setGranularPitch(pitchSpeed);
	} else {
		player->setSpeed(pitchSpeed);
	}

	player->trigger();

	if(slot.muteGroup != MuteGroup::None) {
		player->setActiveSlot(slot.id);
	}

	const size_t playerIndex = static_cast<size_t>(player - &players[0]);
	rt_printf("Play sample %s on player %zu loop=%d pitch=%.2f playmode=%s reversed=%d vel=%d\n",
		slot.sample->getName().c_str(),
		playerIndex,
		loop ? 1 : 0,
		slot.pitchSemitones,
		slot.playMode == Program::SlotPlayMode::Granular
			? ProgramJson::kPlayModeGranular
			: ProgramJson::kPlayModeNormal,
		slot.reversed ? 1 : 0,
		velocity);
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
