#pragma once

#include "Program.h"
#include "SamplePlayerPool.h"

class VoiceAllocator
{
public:
	void init(SamplePlayerPool* pool);

	SamplerVoice* acquire(const Program::Slot& slot);
	void releaseGate(const Program::Slot& slot);
	void stopMuteGroupExceptNote(MuteGroup group, int midiNote);

private:
	void releaseFinishedVoices();
	SamplerVoice* acquireDedicatedPlayer(size_t slotId);
	SamplerVoice* acquireGatePlayer(size_t slotId);
	SamplerVoice* acquireMuteGroupPlayer(const Program::Slot& slot);
	SamplerVoice* findDedicatedPlayerForSlot(size_t slotId) const;
	SamplerVoice* findMuteGroupVoiceForSlot(const Program::Slot& slot) const;
	SamplerVoice* findFreePolyPlayer() const;
	SamplerVoice* findUnassignedPlayer() const;

	SamplePlayerPool* playerPool = nullptr;
};
