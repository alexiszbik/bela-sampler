#include "SamplePlayerPool.h"

#include "MixBusNames.h"

#include "ProgramJson.h"

#include "SamplerLog.h"

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
	SAMPLER_LOG("Play sample %s on player %zu loop=%d pitch=%.2f playmode=%s reversed=%d vel=%d\n",
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
	SAMPLER_LOG("Stop player %zu\n", voiceIndex);
}

void SamplePlayerPool::nextSamples(MixBusArray& mixBuses) {
	for(SamplerVoice& voice : voices) {
		const MixBusIndex busIndex = voice.getBusIndex();
		if(busIndex < kBusMaster || busIndex >= kBusCount) {
			continue;
		}

		MixBusBase& bus = mixBuses.getBus(busIndex);
		const size_t busChannels = bus.getChannelCount();
		voice.nextSamples(bus.getSum(), busChannels, busChannels);
	}
}
