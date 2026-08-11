#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class LogWindow : public juce::Component,
				  private juce::Timer
{
public:
	LogWindow();

	void paint(juce::Graphics& g) override;
	void resized() override;

private:
	void timerCallback() override;
	void trimDisplayedText();

	juce::TextEditor logEditor;
};
