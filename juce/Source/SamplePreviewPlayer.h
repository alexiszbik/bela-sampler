#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>

class SamplePreviewPlayer
{
public:
	void prepare(double sampleRate, int blockSize);
	void play(const juce::File& file);
	void stop();
	void mixInto(float* const* output, int numChannels, int numSamples);

private:
	juce::AudioTransportSource transport;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioBuffer<float> scratchBuffer;
};
