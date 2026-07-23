#pragma once

#include "MuteGroup.h"

#include <cstddef>

struct VoiceBinding
{
	static constexpr size_t kInvalidSlot = static_cast<size_t>(-1);

	size_t slotId = kInvalidSlot;
	size_t activeSlotId = kInvalidSlot;
	bool isMonoOwner = false;
	MuteGroup muteGroup = MuteGroup::None;

	bool isBound() const { return slotId != kInvalidSlot; }
	bool isBoundTo(size_t id) const { return slotId == id; }
	bool isActiveSlot(size_t id) const { return activeSlotId == id; }
	bool isAssigned() const { return isMonoOwner || muteGroup != MuteGroup::None; }
};
