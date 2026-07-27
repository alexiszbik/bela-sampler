#pragma once

#include "MuteGroup.h"
#include "Sample.h"

#include <vector>

struct ProgramSlotDesc;

class Program
{
public:
	enum class SlotMode {
		Poly,
		Mono,
		Gate
	};

	enum class SlotPlayMode {
		Normal,
		Granular
	};

	struct Slot {
		size_t id;
		int midiNote;
		const Sample* sample;
		SlotMode mode = SlotMode::Poly;
		MuteGroup muteGroup = MuteGroup::None;
		float pitchSemitones = 0.f;
		SlotPlayMode playMode = SlotPlayMode::Normal;
		float granularSpeed = 1.f;
		bool reversed = false;
		float volumeDb = 0.f;
		int bus = 0;

		bool isMuteOnly() const { return sample == nullptr; }
	};

	void addSlot(const ProgramSlotDesc& desc, const Sample* sample);

	bool loadFromFile(const std::string& filepath, const std::vector<Sample>& samples);
	
	const std::vector<Slot>& getSlots() const { return slots; }
	size_t getSlotCount() const { return slots.size(); }

private:
	std::vector<Slot> slots;
};
