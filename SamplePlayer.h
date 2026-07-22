#pragma once

#include "Sample.h"

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
	void init(double playingSampleRate);
	void setSample(const Sample* sample);

	void nextSamples(float* buf, size_t bufSize);

	void trigger();
	bool getIsPlaying() const { return isPlaying; }

	void setSpeed(float newSpeed) { speed = newSpeed; }
	void setLoop(bool loopState) { isLoop = loopState; }
	void setPlayMode(EPlayerMode pm);
	void setGranularSpeed(float newSpeed) { granularSpeed = newSpeed; }
	void setGranularPitch(float newPitch) { granularPitch = newPitch; }

private:
	void nextSamplesNormal(float* buf, size_t bufSize);
	void nextSamplesGranular(float* buf, size_t bufSize);
	void spawnGrain(Grain& grain);
	void processGrain(Grain& grain, float* buf, size_t bufSize, double grainReadSpeed);
	float hannEnvelope(double phase) const;
	void resetPlaybackState();
	void resetGranularState();
	void wrapSampleIndex(double& index) const;
	void updateSrSpeed();

	const Sample* sample = nullptr;

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

	EPlayerMode playMode = Normal;
	bool isLoop = false;
	bool isPlaying = false;
};
