#pragma once

#include <juce_audio_formats/juce_audio_formats.h>

class SharedAudioFormatManager
{
public:
	static juce::AudioFormatManager& get();

private:
	SharedAudioFormatManager();
	juce::AudioFormatManager manager;
};
