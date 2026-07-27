#pragma once

class MidiInputDelegate
{
public:
	virtual ~MidiInputDelegate() = default;

	virtual void onNoteOn(int note, int velocity) = 0;
	virtual void onNoteOff(int note) = 0;
	virtual void onControlChange(int controller, int value) = 0;
};
