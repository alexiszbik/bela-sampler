#include "SamplerMidiCollector.h"

SamplerMidiCollector::SamplerMidiCollector(juce::MidiBuffer& buffer) : targetBuffer(buffer) {}

void SamplerMidiCollector::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) {
	(void)source;
	const juce::ScopedLock lock(bufferLock);
	targetBuffer.addEvent(message, 0);
}

void SamplerMidiCollector::swapBuffer(juce::MidiBuffer& destination) {
	const juce::ScopedLock lock(bufferLock);
	destination.swapWith(targetBuffer);
	targetBuffer.clear();
}
