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
	bool loadFromFile(const std::string& filepath, const std::vector<Sample>& samples);
	const Sample* getSampleForNote(int note) const;
	size_t getSlotCount() const { return slots.size(); }

private:
	std::vector<Slot> slots;
};
