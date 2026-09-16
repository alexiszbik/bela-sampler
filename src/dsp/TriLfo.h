#pragma once

class TriLfo {
public:
	void init(double sampleRate);
	void setFrequency(float frequencyHz);
	void setPhase(float phase);
	void reset();

	float process();
	float valueAtPhaseOffset(float offset) const;

private:
	float triangleFromPhase(float phase) const;

	double sampleRate = 44100.0;
	float frequencyHz = 1.0f;
	float phase = 0.0f;
	float phaseIncrement = 0.0f;
};
