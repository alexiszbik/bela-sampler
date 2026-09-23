#pragma once

#include "ProgramEditorBank.h"
#include "ProgramMapJson.h"
#include "ProgramJson.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>

class ProgramGridComponent;
class SamplerPreviewEngine;

class ProgramEditorWindow : public juce::Component,
							private juce::ComboBox::Listener
{
public:
	ProgramEditorWindow();
	~ProgramEditorWindow() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	void loadPrograms(const std::string& programFolder);
	void setPreviewPlayer(SamplerPreviewEngine* previewPlayer);

private:
	void comboBoxChanged(juce::ComboBox* comboBox) override;
	void loadSelectedProgram();
	void updateLayout();
	void saveAllPrograms();
	void reloadAllPrograms();
	void exportSamplesHeader();
	void addSampleWithDialog();
	void markDirty();

	std::string programFolder;
	bool dirty = false;

	juce::ComboBox programSelector;
	juce::TextButton saveButton {"Save all"};
	juce::TextButton reloadButton {"Recharger"};
	juce::TextButton addLayerButton {"+ Layer"};
	juce::TextButton addSampleButton {"+ Sample"};
	juce::TextButton exportHeaderButton {"Export .h"};

	juce::Viewport viewport;
	juce::Component contentContainer;

	std::unique_ptr<ProgramGridComponent> programGrid;

	ProgramEditorBank editorBank;
	std::string currentProgramFileKey;
	SamplerPreviewEngine* previewPlayer = nullptr;

	std::unique_ptr<juce::FileChooser> exportHeaderFileChooser;
	std::unique_ptr<juce::FileChooser> addSampleFileChooser;
};
