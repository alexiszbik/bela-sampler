#pragma once

#include <libraries/Midi/Midi.h>

class SamplerEngine;

class MidiInput
{
public:
	bool setup(SamplerEngine* engine);

private:
	static void onMessage(MidiChannelMessage message, void* arg);

	Midi midi;
	static SamplerEngine* engine;
};
