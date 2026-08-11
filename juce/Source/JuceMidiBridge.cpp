#include "JuceMidiBridge.h"

#include "SamplerLog.h"

void JuceMidiBridge::processMidiBuffer(const juce::MidiBuffer& midiBuffer) {
	if(delegate == nullptr) {
		return;
	}

	for(const auto metadata : midiBuffer) {
		const juce::MidiMessage& message = metadata.getMessage();
		const int channel = message.getChannel() - 1;

		if(message.isNoteOn()) {
			const int velocity = message.getVelocity();
			if(velocity > 0) {
				SAMPLER_LOG("MIDI Note On  ch=%d note=%d vel=%d\n",
					channel,
					message.getNoteNumber(),
					velocity);
				delegate->onNoteOn(message.getNoteNumber(), velocity, channel);
			} else {
				SAMPLER_LOG("MIDI Note Off ch=%d note=%d\n", channel, message.getNoteNumber());
				delegate->onNoteOff(message.getNoteNumber(), channel);
			}
		} else if(message.isNoteOff()) {
			SAMPLER_LOG("MIDI Note Off ch=%d note=%d vel=%d\n",
				channel,
				message.getNoteNumber(),
				message.getVelocity());
			delegate->onNoteOff(message.getNoteNumber(), channel);
		} else if(message.isController()) {
			SAMPLER_LOG("MIDI CC       ch=%d cc=%d val=%d\n",
				channel,
				message.getControllerNumber(),
				message.getControllerValue());
			delegate->onControlChange(message.getControllerNumber(),
				message.getControllerValue(),
				channel);
		} else if(message.isProgramChange()) {
			SAMPLER_LOG("MIDI PC       ch=%d prog=%d\n", channel, message.getProgramChangeNumber());
			delegate->onPgmChange(message.getProgramChangeNumber(), channel);
		}
	}
}
