#pragma once

#include "MidiInputDelegate.h"

#include <mutex>
#include <queue>
#include <vector>

class RtMidiBridge
{
public:
	explicit RtMidiBridge(MidiInputDelegate& delegate);

	void onMessage(double timestamp, const std::vector<unsigned char>& bytes);
	void drain();

private:
	struct Event {
		int status;
		int data1;
		int data2;
	};

	MidiInputDelegate& delegate;
	std::mutex lock;
	std::queue<Event> pending;
};
