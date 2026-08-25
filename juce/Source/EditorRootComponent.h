#pragma once

#include "PreviewAudioHost.h"
#include "ProgramEditorWindow.h"
#include "Sample.h"
#include "waveform/SampleWaveform.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>

class EditorRootComponent : public juce::Component
{
public:
	explicit EditorRootComponent(PreviewAudioHost& inPreviewHost);
	~EditorRootComponent() override = default;

	void resized() override;
	void paint(juce::Graphics& g) override;

	ProgramEditorWindow& getEditor() { return editor; }

private:
	void onSamplePreviewed(const Sample& sample, bool reversed, float volumeDb, const std::string& displayName);

	PreviewAudioHost& previewHost;
	ProgramEditorWindow editor;
	juce::Label waveformTitle;
	SampleWaveform waveformView;
	std::vector<float> monoWaveformScratch;

	static constexpr int kEditorHeightPercent = 65;
};
