#pragma once

#include <string>
#include <vector>

class Sample
{
public:
	bool load(const std::string& filepath);

	void tableRead(double index, float* buf, size_t bufSize) const;

	unsigned int getLength() const { return static_cast<unsigned int>(sampleLength); }
	double getSampleLengthDouble() const { return dSampleLength; }
	size_t getRamBytes() const;
	unsigned int getChannelCount() const { return channelCount; }
	unsigned int getSampleRate() const { return sampleRate; }

	std::string getChannelDescription() const;
	const std::string& getName() const { return name; }

private:
	std::string name;
	std::vector<std::vector<float>> sampleData;

	size_t sampleLength = 0;
	double dSampleLength = 0.0;
	unsigned int channelCount = 0;
	unsigned int sampleRate = 0;
};
