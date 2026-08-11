#pragma once

#include "LogWindow.h"
#include "ProgramEditorWindow.h"
#include "SamplerAudioEngine.h"

#include <juce_gui_basics/juce_gui_basics.h>

class MainWindow : public juce::DocumentWindow
{
public:
	MainWindow(juce::String name, SamplerAudioEngine& audioEngine);

	void closeButtonPressed() override;

private:
	SamplerAudioEngine& engine;
	juce::TabbedComponent tabs;
	LogWindow logWindow;
	ProgramEditorWindow editorWindow;
};
