#pragma once

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
	};

	void addSlot(int midiNote, const Sample* sample, SlotMode mode = SlotMode::Poly);
	bool loadFromFile(const std::string& filepath, const std::vector<Sample>& samples);
	const Slot* getSlotForNote(int note) const;
	size_t getSlotCount() const { return slots.size(); }

private:
	std::vector<Slot> slots;
};
