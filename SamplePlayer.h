#pragma once

#include <string>
#include <vector>

class SamplePlayer
{
public:
	enum EPlayerMode {
		Normal,
		Granular
	};

	struct Grain {
		double readPos = 0.0;
		double phase = 1.0;
	};

public:
	void init(double sr) { playingSampleRate = sr; }

	bool load(const std::string& filepath);

	void nextSamples(float* buf, size_t bufSize);

	unsigned int getLength() const { return static_cast<unsigned int>(sampleLength); }
	size_t getRamBytes() const;

	unsigned int getChannelCount() const { return channelCount; }
	unsigned int getSampleRate() const { return sampleRate; }

	void setSpeed(float newSpeed) { speed = newSpeed; }
	void setPlayMode(EPlayerMode pm) { playMode = pm; }
	void setGranularSpeed(float newSpeed) { granularSpeed = newSpeed; }
	void setGranularPitch(float newPitch) { granularPitch = newPitch; }

	std::string getChannelDescription() const;
	const std::string& getName() const { return name; }

private:
	void tableRead(double index, size_t tableLength, float* buf, size_t bufSize);
	void nextSamplesNormal(float* buf, size_t bufSize);
	void nextSamplesGranular(float* buf, size_t bufSize);
	void spawnGrain(Grain& grain);
	void processGrain(Grain& grain, float* buf, size_t bufSize, double grainReadSpeed);
	float hannEnvelope(double phase) const;
	void resetGranularState();
	void wrapSampleIndex(double& index) const;

	std::string name;
	std::vector<std::vector<float>> sampleData;

	double readPos = 0.0;
	double playingSampleRate = 44100.0;
	double srSpeed = 1.0;

	double grainOutputLength = 0.0;
	double grainPhaseIncrement = 0.0;
	double grainHop = 0.0;
	double grainHopCounter = 0.0;
	double sequentialPos = 0.0;

	Grain grains[2];
	int nextGrainIndex = 0;

	float speed = 1.f;
	float granularSpeed = 1.f;
	float granularPitch = 1.f;

	size_t sampleLength = 0;
	double dSampleLength = 0.0;
	unsigned int channelCount = 0;
	unsigned int sampleRate = 0;

	EPlayerMode playMode = Normal;
};
