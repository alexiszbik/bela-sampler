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
	float getSpeed() const { return speed; }
	void setSpeed(float newSpeed) { speed = newSpeed; }
	void setPlayingSampleRate(double rate) { playingSampleRate = rate; }

	std::string getChannelDescription() const;
	const std::string& getName() const { return name; }

private:
	void tableRead(double index, size_t tableLength, float* buf, size_t bufSize);

	std::string name;
	std::vector<std::vector<float>> sampleData;
	double readPos = 0.0;
	double playingSampleRate = 44100.0;
	float speed = 1.f;
	unsigned int channelCount = 0;
	unsigned int sampleRate = 0;
};
