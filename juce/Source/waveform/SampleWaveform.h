#pragma once

// Adapted from BLEASS sonicwiz SampleWaveform (William DULOT / BLEASS).

#include "WaveformBuffer.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <cstddef>

class SampleWaveform : public juce::Component
{
public:
	SampleWaveform();
	~SampleWaveform() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	void updateSampleBuf(const float* left, size_t bufLength, const float* right = nullptr);
	void updateWindow(float relativeStart, float relativeEnd);
	void setStart(float start);
	void setEnd(float end);
	void setAmplitudeFactor(float factor);
	void setReverse(bool reverse);

private:
	void processSampleData();
	void clearWaveformPaths();
	void buildWaveformPaths();

	const float* sampleDataLeft = nullptr;
	const float* sampleDataRight = nullptr;
	size_t sampleLength = 0;
	float amplitudeFactor = 1.f;
	bool reverse = false;
	bool isStereoDisplay = false;

	static constexpr size_t kResolution = 800;

	WaveformBuffer<float> readRamp {0.f, kResolution};
	WaveformBuffer<float> uiSampleMax[2] = {WaveformBuffer<float>(0.f, kResolution),
		WaveformBuffer<float>(0.f, kResolution)};
	WaveformBuffer<float> uiSampleMin[2] = {WaveformBuffer<float>(0.f, kResolution),
		WaveformBuffer<float>(0.f, kResolution)};

	bool renderSingleLine = false;

	float windowStart = 0.f;
	float windowEnd = 1.f;

	float playStart = 0.f;
	float playEnd = 1.f;

	juce::Path waveformPaths[2];
};
