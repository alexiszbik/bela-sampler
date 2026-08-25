#pragma once

#include "EditorRootComponent.h"
#include "PreviewAudioHost.h"

#include <juce_gui_basics/juce_gui_basics.h>

class MainWindow : public juce::DocumentWindow
{
public:
	MainWindow(juce::String name);
	~MainWindow() override;

	void closeButtonPressed() override;

private:
	PreviewAudioHost previewHost;
	EditorRootComponent rootComponent;
};
