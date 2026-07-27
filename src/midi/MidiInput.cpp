#include "MidiInput.h"

#include <Bela.h>

MidiInputDelegate* MidiInput::delegate = nullptr;

bool MidiInput::setup(MidiInputDelegate* inDelegate) {
	delegate = inDelegate;
	midi.readFrom(0);
	midi.setParserCallback(onMessage);
	rt_printf("MIDI input enabled on port 0\n");
	return delegate != nullptr;
}

void MidiInput::onMessage(MidiChannelMessage message, void* arg) {
	(void)arg;

	if(delegate == nullptr) {
		return;
	}

	switch(message.getType()) {
		case kmmNoteOn: {
			const int note = message.getDataByte(0);
			const int velocity = message.getDataByte(1);
			if(velocity > 0) {
				rt_printf("MIDI Note On  ch=%d note=%d vel=%d\n",
					message.getChannel(), note, velocity);
				delegate->onNoteOn(note, velocity);
			} else {
				rt_printf("MIDI Note Off ch=%d note=%d\n",
					message.getChannel(), note);
				delegate->onNoteOff(note);
			}
			break;
		}
		case kmmNoteOff: {
			rt_printf("MIDI Note Off ch=%d note=%d vel=%d\n",
				message.getChannel(),
				message.getDataByte(0),
				message.getDataByte(1));
			delegate->onNoteOff(message.getDataByte(0));
			break;
		}
		case kmmControlChange: {
			const int controller = message.getDataByte(0);
			const int value = message.getDataByte(1);
			rt_printf("MIDI CC       ch=%d cc=%d val=%d\n",
				message.getChannel(),
				controller,
				value);
			delegate->onControlChange(controller, value);
			break;
		}
		case kmmProgramChange: {
			rt_printf("MIDI PC       ch=%d prog=%d\n",
				message.getChannel(),
				message.getDataByte(0));
			break;
		}
		default:
			break;
	}
}
