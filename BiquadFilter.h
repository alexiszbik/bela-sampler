#pragma once

class BiquadFilter
{
public:
	void init(float sampleRate);
	void setBandpass(float centerFreq, float q);
	void setLowpass(float cutoffFreq, float q);
	void setHighpass(float cutoffFreq, float q);
	float process(float input);
	void reset();

private:
	void normalize();

	float a0 = 1.f;
	float a1 = 0.f;
	float a2 = 0.f;
	float b0 = 1.f;
	float b1 = 0.f;
	float b2 = 0.f;
	float z1 = 0.f;
	float z2 = 0.f;
	float sampleRate = 44100.f;
};
