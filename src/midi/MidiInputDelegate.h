#pragma once

class MidiInputDelegate
{
public:
	virtual ~MidiInputDelegate() = default;

	virtual void onNoteOn(int note, int velocity, int channel) = 0;
	virtual void onNoteOff(int note, int channel) = 0;
	virtual void onControlChange(int controller, int value, int channel) = 0;
	virtual void onPgmChange(int pgm, int channel) = 0;
};
