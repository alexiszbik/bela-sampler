#pragma once

#include "MuteGroup.h"
#include "Sample.h"

#include <vector>

class Program
{
public:
	enum class SlotMode {
		Poly,
		Mono,
		Gate
	};

	struct Slot {
		size_t id;
		int midiNote;
		const Sample* sample;
		SlotMode mode = SlotMode::Poly;
		MuteGroup muteGroup = MuteGroup::None;
	};

	void addSlot(int midiNote, const Sample* sample, SlotMode mode = SlotMode::Poly,
		MuteGroup muteGroup = MuteGroup::None);
	bool loadFromFile(const std::string& filepath, const std::vector<Sample>& samples);
	const Slot* getSlotForNote(int note) const;
	size_t getSlotCount() const { return slots.size(); }

private:
	std::vector<Slot> slots;
};
