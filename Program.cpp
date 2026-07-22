#include "Program.h"

void Program::addSlot(int midiNote, const Sample* sample) {
	slots.push_back({midiNote, sample});
}

const Sample* Program::getSampleForNote(int note) const {
	for(const Slot& slot : slots) {
		if(slot.midiNote == note) {
			return slot.sample;
		}
	}

	return nullptr;
}
