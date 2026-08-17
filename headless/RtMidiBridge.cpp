#include "RtMidiBridge.h"

RtMidiBridge::RtMidiBridge(MidiInputDelegate& inDelegate)
	: delegate(inDelegate) {
}

void RtMidiBridge::onMessage(double timestamp, const std::vector<unsigned char>& bytes) {
	(void)timestamp;
	if(bytes.empty()) {
		return;
	}

	Event event;
	event.status = bytes[0];
	event.data1 = bytes.size() > 1 ? bytes[1] : 0;
	event.data2 = bytes.size() > 2 ? bytes[2] : 0;

	std::lock_guard<std::mutex> guard(lock);
	pending.push(event);
}

void RtMidiBridge::drain() {
	std::queue<Event> local;
	{
		std::lock_guard<std::mutex> guard(lock);
		local.swap(pending);
	}

	while(!local.empty()) {
		const Event event = local.front();
		local.pop();

		const int statusByte = event.status & 0xF0;
		const int channel = event.status & 0x0F;

		switch(statusByte) {
			case 0x80:
				delegate.onNoteOff(event.data1, channel);
				break;
			case 0x90:
				if(event.data2 > 0) {
					delegate.onNoteOn(event.data1, event.data2, channel);
				} else {
					delegate.onNoteOff(event.data1, channel);
				}
				break;
			case 0xB0:
				delegate.onControlChange(event.data1, event.data2, channel);
				break;
			case 0xC0:
				delegate.onPgmChange(event.data1, channel);
				break;
			default:
				break;
		}
	}
}
