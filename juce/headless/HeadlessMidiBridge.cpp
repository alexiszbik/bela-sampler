#include "HeadlessMidiBridge.h"

#include "SamplerLog.h"

void HeadlessMidiBridge::processMidiBuffer(const juce::MidiBuffer& midiBuffer) {
	if(delegate == nullptr) {
		return;
	}

	for(const juce::MidiMessageMetadata metadata : midiBuffer) {
		const juce::MidiMessage& message = metadata.getMessage();
		const int channel = message.getChannel();

		if(message.isNoteOn()) {
			const int velocity = message.getVelocity();
			if(velocity > 0) {
				delegate->onNoteOn(message.getNoteNumber(), velocity, channel);
			} else {
				delegate->onNoteOff(message.getNoteNumber(), channel);
			}
		} else if(message.isNoteOff()) {
			delegate->onNoteOff(message.getNoteNumber(), channel);
		} else if(message.isController()) {
			delegate->onControlChange(message.getControllerNumber(), message.getControllerValue(), channel);
		} else if(message.isProgramChange()) {
			delegate->onPgmChange(message.getProgramChangeNumber(), channel);
		}
	}
}
