#pragma once

#include "DispatchGroup.h"
#include "MuteGroup.h"
#include "MixBusNames.h"
#include "Sample.h"
#include "QuadDispatch.h"
#include "SlotDispatch.h"

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
		MixBusIndex bus = kBusMaster;
		float pan = 0.f;
		SlotDispatch dispatch = SlotDispatch::Front;
		DispatchGroup dispatchGroup = DispatchGroup::None;
		mutable QuadDispatch localDispatchState;

		bool isMuteOnly() const { return sample == nullptr; }
	};

	void addSlot(const ProgramSlotDesc& desc, const Sample* sample);

	bool loadFromFile(const std::string& filepath, const std::vector<Sample>& samples);

	std::vector<Slot>& getSlots() { return slots; }
	const std::vector<Slot>& getSlots() const { return slots; }
	size_t getSlotCount() const { return slots.size(); }

	QuadDispatch& dispatchStateFor(Slot& slot);

private:
	static constexpr size_t kDispatchGroupCount = 4;

	std::vector<Slot> slots;
	QuadDispatch dispatchGroups[kDispatchGroupCount];
};
