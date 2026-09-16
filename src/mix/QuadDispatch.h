#pragma once

#include "SlotDispatch.h"

#include <cstddef>

struct DispatchChannels {
	size_t left = 0;
	size_t right = 1;
};

class QuadDispatch {
public:
	static constexpr size_t kMaxChannels = 4;

	DispatchChannels resolve(SlotDispatch mode, bool isStereo, size_t busChannelCount);

private:
	DispatchChannels leftChannelToOutput(size_t leftChannel, bool isStereo) const;
	size_t randomLeftChannel() const;

	size_t currentIndex = 0;
};
