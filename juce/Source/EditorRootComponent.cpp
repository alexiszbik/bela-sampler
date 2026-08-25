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
	: previewHost(inPreviewHost) {
	waveformTitle.setText("Last played sample", juce::dontSendNotification);
	waveformTitle.setJustificationType(juce::Justification::centredLeft);
	waveformTitle.setColour(juce::Label::textColourId, juce::Colours::lightgrey);

	addAndMakeVisible(editor);
	addAndMakeVisible(waveformTitle);
	addAndMakeVisible(waveformView);

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
	waveformView.setBounds(waveformArea);
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

	const float* waveformData = sample.getChannelSamples(0);
	if(waveformData == nullptr) {
		return;
	}

	if(sample.getChannelCount() > 1) {
		monoWaveformScratch.resize(length);
		for(unsigned int i = 0; i < length; ++i) {
			float sum = 0.f;
			for(unsigned int channel = 0; channel < sample.getChannelCount(); ++channel) {
				const float* channelData = sample.getChannelSamples(channel);
				if(channelData != nullptr) {
					sum += channelData[i];
				}
			}
			monoWaveformScratch[i] = sum / static_cast<float>(sample.getChannelCount());
		}
		waveformData = monoWaveformScratch.data();
	}

	waveformView.setReverse(reversed);
	waveformView.setAmplitudeFactor(previewGainFromVolumeDb(volumeDb));
	waveformView.updateWindow(0.f, 1.f);
	waveformView.setStart(0.f);
	waveformView.setEnd(1.f);
	waveformView.updateSampleBuf(waveformData, length);
}
