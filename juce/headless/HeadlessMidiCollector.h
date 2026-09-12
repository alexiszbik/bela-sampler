#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

class HeadlessMidiCollector : public juce::MidiInputCallback
{
public:
	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
	void swapBuffer(juce::MidiBuffer& destination);

private:
	juce::MidiBuffer incomingBuffer;
	juce::CriticalSection bufferLock;
};
