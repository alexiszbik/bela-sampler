#pragma once

#include "Sample.h"

#include <vector>

class Program
{
public:
	struct Slot {
		int midiNote;
		const Sample* sample;
	};

	void addSlot(int midiNote, const Sample* sample);
	const Sample* getSampleForNote(int note) const;
	size_t getSlotCount() const { return slots.size(); }

private:
	std::vector<Slot> slots;
};
