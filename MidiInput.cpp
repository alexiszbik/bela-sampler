#include "MidiInput.h"

#include "SamplerEngine.h"

SamplerEngine* MidiInput::engine = nullptr;

bool MidiInput::setup(SamplerEngine* inEngine) {
	engine = inEngine;
	midi.readFrom(0);
	midi.setParserCallback(onMessage);
	rt_printf("MIDI input enabled on port 0\n");
	return true;
}

void MidiInput::onMessage(MidiChannelMessage message, void* arg) {
	switch(message.getType()) {
		case kmmNoteOn: {
			const int note = message.getDataByte(0);
			const int velocity = message.getDataByte(1);
			if(velocity > 0) {
				rt_printf("MIDI Note On  ch=%d note=%d vel=%d\n",
					message.getChannel(), note, velocity);
				if(engine != nullptr) {
					engine->onNoteOn(note, velocity);
				}
			} else {
				rt_printf("MIDI Note Off ch=%d note=%d\n",
					message.getChannel(), note);
				if(engine != nullptr) {
					engine->onNoteOff(note);
				}
			}
			break;
		}
		case kmmNoteOff: {
			rt_printf("MIDI Note Off ch=%d note=%d vel=%d\n",
				message.getChannel(),
				message.getDataByte(0),
				message.getDataByte(1));
			if(engine != nullptr) {
				engine->onNoteOff(message.getDataByte(0));
			}
			break;
		}
		case kmmControlChange: {
			rt_printf("MIDI CC       ch=%d cc=%d val=%d\n",
				message.getChannel(),
				message.getDataByte(0),
				message.getDataByte(1));
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
