#include "LogBuffer.h"

#include <algorithm>
#include <cstring>

LogBuffer& LogBuffer::getInstance() {
	static LogBuffer instance;
	return instance;
}

void LogBuffer::push(const char* text) {
	if(text == nullptr) {
		return;
	}

	const size_t textLen = std::strlen(text);
	if(textLen == 0) {
		return;
	}

	const size_t maxCopy = std::min(textLen, kMaxLineLength - 1);
	size_t write = writePos.load(std::memory_order_relaxed);
	const size_t read = readPos.load(std::memory_order_acquire);

	for(size_t i = 0; i < maxCopy; ++i) {
		buffer[write % kCapacity] = text[i];
		write++;
	}

	const size_t used = write - read;
	if(used > kCapacity) {
		readPos.store(write - kCapacity, std::memory_order_release);
	}

	writePos.store(write, std::memory_order_release);
}

size_t LogBuffer::drainTo(char* out, size_t maxBytes) {
	if(out == nullptr || maxBytes == 0) {
		return 0;
	}

	const size_t currentWrite = writePos.load(std::memory_order_acquire);
	size_t read = readPos.load(std::memory_order_relaxed);

	if(read >= currentWrite) {
		return 0;
	}

	const size_t available = currentWrite - read;
	const size_t toRead = std::min(available, maxBytes);

	for(size_t i = 0; i < toRead; ++i) {
		out[i] = buffer[(read + i) % kCapacity];
	}

	readPos.store(read + toRead, std::memory_order_release);
	return toRead;
}
