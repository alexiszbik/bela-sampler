#include "SamplePlayerPool.h"

#include "ProgramJson.h"

#include <Bela.h>

void SamplePlayerPool::init(double sampleRate, size_t count) {
	voices.resize(count);
	for(SamplerVoice& voice : voices) {
		voice.init(sampleRate);
	}
}

void SamplePlayerPool::playOn(SamplerVoice* voice, const Program::Slot& slot, int velocity) {
	if(voice == nullptr || slot.sample == nullptr) {
		return;
	}

	voice->playOn(slot, velocity);

	const size_t voiceIndex = static_cast<size_t>(voice - &voices[0]);
	const bool loop = slot.mode == Program::SlotMode::Gate;
	rt_printf("Play sample %s on player %zu loop=%d pitch=%.2f playmode=%s reversed=%d vel=%d\n",
		slot.sample->getName().c_str(),
		voiceIndex,
		loop ? 1 : 0,
		slot.pitchSemitones,
		slot.playMode == Program::SlotPlayMode::Granular
			? ProgramJson::kPlayModeGranular
			: ProgramJson::kPlayModeNormal,
		slot.reversed ? 1 : 0,
		velocity);
}

void SamplePlayerPool::stop(SamplerVoice* voice) {
	if(voice == nullptr) {
		return;
	}

	voice->stop();

	const size_t voiceIndex = static_cast<size_t>(voice - &voices[0]);
	rt_printf("Stop player %zu\n", voiceIndex);
}

void SamplePlayerPool::nextSamples(float* buf, size_t bufSize) {
	for(SamplerVoice& voice : voices) {
		voice.nextSamples(buf, bufSize);
	}
}
