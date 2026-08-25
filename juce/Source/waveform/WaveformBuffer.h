#pragma once

#include <cstddef>
#include <cstdlib>

template<typename T = float>
class WaveformBuffer {
public:
	WaveformBuffer() = default;

	WaveformBuffer(T initValue, size_t bufferSize)
		: size(bufferSize) {
		data = static_cast<T*>(std::calloc(bufferSize, sizeof(T)));
		if(data != nullptr && initValue != T {}) {
			for(size_t i = 0; i < bufferSize; ++i) {
				data[i] = initValue;
			}
		}
	}

	~WaveformBuffer() {
		std::free(data);
	}

	WaveformBuffer(const WaveformBuffer&) = delete;
	WaveformBuffer& operator=(const WaveformBuffer&) = delete;

	operator T*() { return data; }

	T& operator[](size_t index) { return data[index]; }

private:
	T* data = nullptr;
	size_t size = 0;
};
