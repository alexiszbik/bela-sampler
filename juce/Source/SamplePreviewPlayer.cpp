#include "SamplePreviewPlayer.h"

#include "SamplerLog.h"
#include "SharedAudioFormatManager.h"

void SamplePreviewPlayer::prepare(double sampleRate, int blockSize) {
	transport.prepareToPlay(blockSize, sampleRate);
	scratchBuffer.setSize(2, blockSize);
}

void SamplePreviewPlayer::stop() {
	transport.stop();
	transport.setSource(nullptr);
	readerSource.reset();
}

void SamplePreviewPlayer::play(const juce::File& file) {
	stop();

	if(!file.existsAsFile()) {
		SAMPLER_LOG("Preview: file not found: %s\n", file.getFullPathName().toRawUTF8());
		return;
	}

	std::unique_ptr<juce::AudioFormatReader> reader(
		SharedAudioFormatManager::get().createReaderFor(file));
	if(reader == nullptr) {
		SAMPLER_LOG("Preview: could not read: %s\n", file.getFullPathName().toRawUTF8());
		return;
	}

	const double fileSampleRate = reader->sampleRate;
	readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
	transport.setSource(readerSource.get(), 0, nullptr, fileSampleRate);
	transport.start();
}

void SamplePreviewPlayer::mixInto(float* const* output, int numChannels, int numSamples) {
	if(numChannels <= 0 || numSamples <= 0 || output == nullptr) {
		return;
	}

	if(!transport.isPlaying()) {
		return;
	}

	const int channelsToUse = juce::jmin(numChannels, scratchBuffer.getNumChannels());
	scratchBuffer.setSize(channelsToUse, numSamples, false, false, true);
	scratchBuffer.clear();

	juce::AudioSourceChannelInfo info(scratchBuffer);
	transport.getNextAudioBlock(info);

	for(int channel = 0; channel < channelsToUse; ++channel) {
		if(output[channel] == nullptr) {
			continue;
		}

		juce::FloatVectorOperations::add(output[channel], scratchBuffer.getReadPointer(channel), numSamples);
	}
}
