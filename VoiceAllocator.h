#pragma once

#include "Program.h"
#include "SamplePlayerPool.h"

class VoiceAllocator
{
public:
	void init(SamplePlayerPool* pool);

	SamplerVoice* acquire(const Program::Slot& slot);
	void releaseGate(const Program::Slot& slot);
	void stopMuteGroup(MuteGroup group);

private:
	void releaseFinishedMonoVoices();
	SamplerVoice* acquireDedicatedPlayer(size_t slotId);
	SamplerVoice* acquireGatePlayer(size_t slotId);
	SamplerVoice* acquireMuteGroupPlayer(MuteGroup group);
	SamplerVoice* findDedicatedPlayerForSlot(size_t slotId) const;
	SamplerVoice* findMuteGroupPlayer(MuteGroup group) const;
	SamplerVoice* findFreePolyPlayer() const;
	SamplerVoice* findUnassignedPlayer() const;

	SamplePlayerPool* playerPool = nullptr;
};
