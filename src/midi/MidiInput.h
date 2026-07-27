#pragma once

#include "MidiInputDelegate.h"

#include <libraries/Midi/Midi.h>

class MidiInput
{
public:
	bool setup(MidiInputDelegate* delegate);

private:
	static void onMessage(MidiChannelMessage message, void* arg);

	Midi midi;
	static MidiInputDelegate* delegate;
};
