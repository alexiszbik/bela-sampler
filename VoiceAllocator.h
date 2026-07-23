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
	SamplePlayer* findDedicatedPlayerForSlot(size_t slotId) const;
	SamplePlayer* findFreePolyPlayer() const;

	SamplePlayerPool* playerPool = nullptr;
};
