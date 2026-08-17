#pragma once

#include <array>
#include <atomic>
#include <cstddef>

class LogBuffer
{
public:
	static constexpr size_t kCapacity = 8192;
	static constexpr size_t kMaxLineLength = 256;

	static LogBuffer& getInstance();

	void push(const char* text);
	size_t drainTo(char* out, size_t maxBytes);

private:
	LogBuffer() = default;

	std::array<char, kCapacity> buffer {};
	std::atomic<size_t> writePos {0};
	std::atomic<size_t> readPos {0};
};
