#pragma once

#include "Program.h"
#include "SamplePlayerPool.h"

class VoiceAllocator
{
public:
	void init(SamplePlayerPool* pool);

	SamplePlayer* acquire(const Program::Slot& slot);
	void releaseGate(const Program::Slot& slot);

private:
	SamplePlayer* acquireDedicatedPlayer(size_t slotId);
	SamplePlayer* acquireMuteGroupPlayer(MuteGroup group);
	SamplePlayer* findDedicatedPlayerForSlot(size_t slotId) const;
	SamplePlayer* findMuteGroupPlayer(MuteGroup group) const;
	SamplePlayer* findFreePolyPlayer() const;
	SamplePlayer* findUnassignedPlayer() const;

	SamplePlayerPool* playerPool = nullptr;
};
