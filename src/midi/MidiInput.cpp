#include "MidiInput.h"

#include "SamplerLog.h"

#include <Bela.h>

MidiInputDelegate* MidiInput::delegate = nullptr;

bool MidiInput::setup(MidiInputDelegate* inDelegate) {
	delegate = inDelegate;
	midi.readFrom(0);
	midi.setParserCallback(onMessage);
	SAMPLER_LOG("MIDI input enabled on port 0\n");
	return delegate != nullptr;
}

void MidiInput::onMessage(MidiChannelMessage message, void* arg) {
	(void)arg;

	if(delegate == nullptr) {
		return;
	}

	const int channel = message.getChannel();

	switch(message.getType()) {
		case kmmNoteOn: {
			const int note = message.getDataByte(0);
			const int velocity = message.getDataByte(1);
			if(velocity > 0) {
				SAMPLER_LOG("MIDI Note On  ch=%d note=%d vel=%d\n", channel, note, velocity);
				delegate->onNoteOn(note, velocity, channel);
			} else {
				SAMPLER_LOG("MIDI Note Off ch=%d note=%d\n", channel, note);
				delegate->onNoteOff(note, channel);
			}
			break;
		}
		case kmmNoteOff: {
			SAMPLER_LOG("MIDI Note Off ch=%d note=%d vel=%d\n",
				channel,
				message.getDataByte(0),
				message.getDataByte(1));
			delegate->onNoteOff(message.getDataByte(0), channel);
			break;
		}
		case kmmControlChange: {
			const int controller = message.getDataByte(0);
			const int value = message.getDataByte(1);
			SAMPLER_LOG("MIDI CC       ch=%d cc=%d val=%d\n", channel, controller, value);
			delegate->onControlChange(controller, value, channel);
			break;
		}
		case kmmProgramChange: {
			const int program = message.getDataByte(0);
			SAMPLER_LOG("MIDI PC       ch=%d prog=%d\n", channel, program);
			delegate->onPgmChange(program, channel);
			break;
		}
		default:
			break;
	}
}
