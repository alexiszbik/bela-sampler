#include "EditorRootComponent.h"

#include "GainHelper.h"
#include "SamplerDesktopPaths.h"

namespace {
constexpr float kPreviewVelocity = 100.f;

float previewGainFromVolumeDb(float volumeDb) {
	const float velocityGain = kPreviewVelocity / 127.f;
	return velocityGain * velocityGain * dBtoRMS(volumeDb);
}
}

EditorRootComponent::EditorRootComponent(PreviewAudioHost& inPreviewHost)
	: previewHost(inPreviewHost),
	  peakMeter(inPreviewHost.getOutputLevels()) {
	waveformTitle.setText("Last played sample", juce::dontSendNotification);
	waveformTitle.setJustificationType(juce::Justification::centredLeft);
	waveformTitle.setColour(juce::Label::textColourId, juce::Colours::lightgrey);

	addAndMakeVisible(editor);
	addAndMakeVisible(waveformTitle);
	addAndMakeVisible(waveformView);
	addAndMakeVisible(peakMeter);

	editor.setPreviewPlayer(&previewHost.getPlayer());
	editor.loadPrograms(SamplerDesktopPaths::getProgramFolder());

	previewHost.getPlayer().onSamplePreviewed = [this](const Sample& sample,
		bool reversed,
		float volumeDb,
		const std::string& displayName) {
		onSamplePreviewed(sample, reversed, volumeDb, displayName);
	};
}

void EditorRootComponent::paint(juce::Graphics& g) {
	g.fillAll(juce::Colour(0xff1a1a1a));

	const int splitY = juce::roundToInt(static_cast<float>(getHeight()) * kEditorHeightPercent / 100.f);
	g.setColour(juce::Colour(0xff404040));
	g.fillRect(0, splitY - 1, getWidth(), 2);
}

void EditorRootComponent::resized() {
	auto bounds = getLocalBounds();
	const int editorHeight = juce::roundToInt(static_cast<float>(bounds.getHeight()) * kEditorHeightPercent / 100.f);

	editor.setBounds(bounds.removeFromTop(editorHeight));

	auto waveformArea = bounds.reduced(8);
	waveformTitle.setBounds(waveformArea.removeFromTop(22));

	auto meterArea = waveformArea.removeFromRight(kPeakMeterWidth);
	peakMeter.setBounds(meterArea);
	waveformView.setBounds(waveformArea.reduced(0, 0).withTrimmedRight(8));
}

void EditorRootComponent::onSamplePreviewed(const Sample& sample,
	bool reversed,
	float volumeDb,
	const std::string& displayName) {
	waveformTitle.setText(displayName.empty() ? "Last played sample" : juce::String(displayName),
		juce::dontSendNotification);

	const auto length = sample.getLength();
	if(length == 0) {
		return;
	}

	const float* leftData = sample.getChannelSamples(0);
	if(leftData == nullptr) {
		return;
	}

	const float* rightData = nullptr;
	if(sample.getChannelCount() > 1) {
		rightData = sample.getChannelSamples(1);
	}

	waveformView.setReverse(reversed);
	waveformView.setAmplitudeFactor(previewGainFromVolumeDb(volumeDb));
	waveformView.updateWindow(0.f, 1.f);
	waveformView.setStart(0.f);
	waveformView.setEnd(1.f);
	waveformView.updateSampleBuf(leftData, length, rightData);
}
