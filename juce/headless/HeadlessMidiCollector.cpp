#include "HeadlessMidiCollector.h"

void HeadlessMidiCollector::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) {
	(void)source;

	const juce::ScopedLock lock(bufferLock);
	incomingBuffer.addEvent(message, static_cast<int>(incomingBuffer.getNumEvents()));
}

void HeadlessMidiCollector::swapBuffer(juce::MidiBuffer& destination) {
	const juce::ScopedLock lock(bufferLock);
	destination.swapWith(incomingBuffer);
	incomingBuffer.clear();
}
