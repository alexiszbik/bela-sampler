#pragma once

#include <string>
#include <vector>

class SamplePlayer
{
public:
	bool load(const std::string& filepath);

	void nextSamples(float* buf, size_t bufSize);

	unsigned int getLength() const;
	unsigned int getChannelCount() const { return channelCount; }
	unsigned int getSampleRate() const { return sampleRate; }

	std::string getChannelDescription() const;
	const std::string& getName() const { return name; }

private:
	std::string name;
	std::vector<std::vector<float>> sampleData;
	unsigned int readPtr = 0;
	unsigned int channelCount = 0;
	unsigned int sampleRate = 0;
};
