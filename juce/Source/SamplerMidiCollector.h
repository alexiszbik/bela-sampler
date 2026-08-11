#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

class SamplerMidiCollector : public juce::MidiInputCallback
{
public:
	explicit SamplerMidiCollector(juce::MidiBuffer& buffer);

	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
	void swapBuffer(juce::MidiBuffer& destination);

private:
	juce::MidiBuffer& targetBuffer;
	juce::CriticalSection bufferLock;
};
