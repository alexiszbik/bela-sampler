#pragma once

#include "MidiInputDelegate.h"

#include <juce_audio_basics/juce_audio_basics.h>

class HeadlessMidiBridge
{
public:
	void setDelegate(MidiInputDelegate* inDelegate) { delegate = inDelegate; }
	void processMidiBuffer(const juce::MidiBuffer& midiBuffer);

private:
	MidiInputDelegate* delegate = nullptr;
};
