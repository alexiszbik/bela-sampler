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

		bool isMuteOnly() const { return sample == nullptr; }
	};

	void addSlot(int midiNote, const Sample* sample, SlotMode mode = SlotMode::Poly,
		MuteGroup muteGroup = MuteGroup::None, float pitchSemitones = 0.f,
		SlotPlayMode playMode = SlotPlayMode::Normal, float granularSpeed = 1.f, bool reversed = false);
	bool loadFromFile(const std::string& filepath, const std::vector<Sample>& samples);
	const Slot* getSlotForNote(int note) const;
	size_t getSlotCount() const { return slots.size(); }

private:
	std::vector<Slot> slots;
};
