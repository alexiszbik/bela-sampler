#include "StereoPeakMeter.h"

#include "PeakMeterDisplay.h"

StereoPeakMeter::StereoPeakMeter(PreviewOutputLevels& inLevels)
	: levels(inLevels) {
	startTimerHz(PeakMeterDisplay::kRefreshHz);
}

StereoPeakMeter::~StereoPeakMeter() {
	stopTimer();
}

void StereoPeakMeter::timerCallback() {
	levels.fetchLevels(rmsL, rmsR, peakHoldL, peakHoldR);
	repaint();
}

juce::Colour StereoPeakMeter::colourForLevel(float normalisedLevel) {
	switch(PeakMeterDisplay::colourForLevel(normalisedLevel)) {
		case PeakMeterDisplay::LevelColour::Red:
			return juce::Colour(0xffe74c3c);
		case PeakMeterDisplay::LevelColour::Yellow:
			return juce::Colour(0xfff1c40f);
		case PeakMeterDisplay::LevelColour::Green:
		default:
			return juce::Colour(0xff2ecc71);
	}
}

void StereoPeakMeter::drawChannelMeter(juce::Graphics& g,
	juce::Rectangle<float> bounds,
	float rms,
	float peakHold,
	const juce::String& label) const {
	auto peakTextArea = bounds.removeFromTop(kPeakTextHeight);
	auto channelLabelArea = bounds.removeFromBottom(kLabelHeight);
	const auto meterBounds = bounds;

	g.setColour(juce::Colour(0xff2a2a2a));
	g.fillRoundedRectangle(meterBounds, 3.f);
	g.setColour(juce::Colour(0xff505050));
	g.drawRoundedRectangle(meterBounds, 3.f, 1.f);

	const float meterHeight = meterBounds.getHeight();
	const float normalisedRms = PeakMeterDisplay::rmsToNormalised(rms);
	const float normalisedPeak = PeakMeterDisplay::peakToNormalised(peakHold);

	if(normalisedRms > 0.f) {
		auto fillArea = meterBounds.withTop(meterBounds.getBottom() - meterHeight * normalisedRms);
		g.setColour(colourForLevel(normalisedRms));
		g.fillRoundedRectangle(fillArea.reduced(2.f, 0.f), 2.f);
	}

	if(normalisedPeak > 0.02f) {
		const float holdY = meterBounds.getBottom() - meterHeight * normalisedPeak;
		g.setColour(juce::Colours::white.withAlpha(0.85f));
		g.fillRect(meterBounds.getX() + 3.f, holdY - 1.f, meterBounds.getWidth() - 6.f, 2.f);
	}

	g.setColour(juce::Colours::white);
	g.setFont(juce::FontOptions(10.f));
	g.drawText(PeakMeterDisplay::formatPeakDb(peakHold), peakTextArea, juce::Justification::centred, false);

	g.setColour(juce::Colours::lightgrey);
	g.setFont(juce::FontOptions(11.f));
	g.drawText(label, channelLabelArea, juce::Justification::centred, false);
}

void StereoPeakMeter::paint(juce::Graphics& g) {
	auto bounds = getLocalBounds().toFloat().reduced(2.f);
	const float channelWidth = (bounds.getWidth() - kChannelGap) * 0.5f;

	auto leftBounds = bounds.removeFromLeft(channelWidth);
	bounds.removeFromLeft(kChannelGap);
	auto rightBounds = bounds;

	drawChannelMeter(g, leftBounds, rmsL, peakHoldL, "L");
	drawChannelMeter(g, rightBounds, rmsR, peakHoldR, "R");
}
