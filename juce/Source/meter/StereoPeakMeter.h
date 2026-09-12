#pragma once

#include "PreviewOutputLevels.h"

#include <juce_gui_basics/juce_gui_basics.h>

class StereoPeakMeter : public juce::Component,
	private juce::Timer
{
public:
	explicit StereoPeakMeter(PreviewOutputLevels& inLevels);
	~StereoPeakMeter() override;

	void paint(juce::Graphics& g) override;

private:
	void timerCallback() override;

	static float rmsToNormalised(float rms);
	static float peakToNormalised(float peak);
	static juce::Colour colourForLevel(float normalisedLevel);

	void drawChannelMeter(juce::Graphics& g,
		juce::Rectangle<float> bounds,
		float rms,
		float peakHold,
		const juce::String& label) const;

	PreviewOutputLevels& levels;
	float rmsL = 0.f;
	float rmsR = 0.f;
	float peakHoldL = 0.f;
	float peakHoldR = 0.f;

	static constexpr int kRefreshHz = 30;
};
