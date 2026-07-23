#pragma once

#include <cstddef>

struct VoiceBinding
{
	static constexpr size_t kInvalidSlot = static_cast<size_t>(-1);

	size_t slotId = kInvalidSlot;
	bool isMonoOwner = false;

	bool isBound() const { return slotId != kInvalidSlot; }
	bool isBoundTo(size_t id) const { return slotId == id; }
};
