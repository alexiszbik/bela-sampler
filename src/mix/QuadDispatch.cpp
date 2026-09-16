#include "QuadDispatch.h"

#include <cstdlib>

DispatchChannels QuadDispatch::resolve(SlotDispatch mode, bool isStereo, size_t busChannelCount) {
	if(busChannelCount == 0) {
		return {};
	}

	size_t leftChannel = currentIndex;

	switch(mode) {
		case SlotDispatch::Random:
			leftChannel = randomLeftChannel();
			break;

		case SlotDispatch::Forward:
			leftChannel = currentIndex;
			currentIndex = (currentIndex + 1) % kMaxChannels;
			break;

		case SlotDispatch::Backward:
			leftChannel = (currentIndex + kMaxChannels - 1) % kMaxChannels;
			currentIndex = leftChannel;
			break;

		default:
			return {};
	}

	return leftChannelToOutput(leftChannel, isStereo);
}

DispatchChannels QuadDispatch::leftChannelToOutput(size_t leftChannel, bool isStereo) const {
	DispatchChannels channels;
	channels.left = leftChannel % kMaxChannels;

	if(isStereo) {
		channels.right = (channels.left + 1) % kMaxChannels;
	} else {
		channels.right = channels.left;
	}

	return channels;
}

size_t QuadDispatch::randomLeftChannel() const {
	return static_cast<size_t>(std::rand()) % kMaxChannels;
}
