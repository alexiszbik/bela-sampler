#include "Sample.h"

#include <cmath>

#if defined(SAMPLER_DESKTOP)
#include <juce_audio_formats/juce_audio_formats.h>
#else
#include <libraries/AudioFile/AudioFile.h>
#include <libraries/sndfile/sndfile.h>
#endif

namespace {
std::string getFileName(const std::string& filepath) {
	const size_t pos = filepath.find_last_of('/');
	if(pos == std::string::npos) {
		return filepath;
	}

	return filepath.substr(pos + 1);
}
}

std::string Sample::getChannelDescription() const {
	switch(channelCount)
	{
		case 1: return "mono";
		case 2: return "stereo";
		default: return std::to_string(channelCount) + " channels";
	}
}

bool Sample::load(const std::string& filepath, const std::string& sampleName) {
#if defined(SAMPLER_DESKTOP)
	juce::AudioFormatManager formatManager;
	formatManager.registerBasicFormats();

	const juce::File audioFile(filepath);
	std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(audioFile));
	if(reader == nullptr) {
		return false;
	}

	const int numChannels = static_cast<int>(reader->numChannels);
	if(numChannels <= 0) {
		return false;
	}

	sampleRate = static_cast<unsigned int>(reader->sampleRate);
	sampleLength = static_cast<size_t>(reader->lengthInSamples);
	dSampleLength = static_cast<double>(sampleLength);
	channelCount = static_cast<unsigned int>(numChannels);

	sampleData.assign(static_cast<size_t>(numChannels), std::vector<float>(sampleLength));

	std::vector<float*> channelPointers(static_cast<size_t>(numChannels));
	for(int channel = 0; channel < numChannels; ++channel) {
		channelPointers[static_cast<size_t>(channel)] = sampleData[static_cast<size_t>(channel)].data();
	}

	if(!reader->read(channelPointers.data(), numChannels, 0, static_cast<int>(sampleLength))) {
		return false;
	}

	name = sampleName.empty() ? getFileName(filepath) : sampleName;
	return sampleLength > 0;
#else
	const int numChannels = AudioFileUtilities::getNumChannels(filepath);
	if(numChannels <= 0) {
		return false;
	}

	SF_INFO sfinfo = {0};
	SNDFILE* sndfile = sf_open(filepath.c_str(), SFM_READ, &sfinfo);
	if(!sndfile) {
		return false;
	}

	sampleRate = static_cast<unsigned int>(sfinfo.samplerate);
	sf_close(sndfile);

	sampleData = AudioFileUtilities::load(filepath);
	if(sampleData.empty() || sampleData[0].empty()) {
		return false;
	}

	channelCount = static_cast<unsigned int>(numChannels);
	name = sampleName.empty() ? getFileName(filepath) : sampleName;
	sampleLength = sampleData[0].size();
	dSampleLength = static_cast<double>(sampleLength);
	return true;
#endif
}

void Sample::tableRead(double index, float* buf, size_t bufSize, bool loop) const {
	if(sampleLength == 0) {
		return;
	}

	const double p = index;
	const double q = std::floor(p);
	const double r = p - q;

	if(!loop && (q >= static_cast<double>(sampleLength) || q < 0.0)) {
		return;
	}

	int nextIndex = static_cast<int>(q) + 1;
	if(nextIndex >= static_cast<int>(sampleLength)) {
		nextIndex = loop ? 0 : static_cast<int>(sampleLength) - 1;
	} else if(nextIndex < 0) {
		nextIndex = loop ? static_cast<int>(sampleLength) - 1 : 0;
	}

	const int currentIndex = static_cast<int>(q);

	for(size_t channel = 0; channel < bufSize; channel++) {
		const unsigned int srcChan = channel % channelCount;
		const float sample0 = sampleData[srcChan][currentIndex];
		const float sample1 = sampleData[srcChan][nextIndex];
		buf[channel] += static_cast<float>((1.0 - r) * sample0 + r * sample1);
	}
}

size_t Sample::getRamBytes() const {
	return sampleLength * channelCount * sizeof(float);
}
