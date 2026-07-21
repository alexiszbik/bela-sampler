#include "SamplePlayer.h"

#include <libraries/AudioFile/AudioFile.h>
#include <libraries/sndfile/sndfile.h>

std::string SamplePlayer::getChannelDescription() const {
	switch(channelCount)
	{
		case 1: return "mono";
		case 2: return "stereo";
		default: return std::to_string(channelCount) + " channels";
	}
}

bool SamplePlayer::load(const std::string& filepath) {
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
	name = filepath;
	readPtr = 0;
	return true;
}

void SamplePlayer::nextSamples(float* buf, size_t bufSize) {
	if(sampleData.empty() || sampleData[0].empty()) {
		return;
	}

	for (size_t chan = 0; chan < bufSize; chan++) {
		buf[chan] += sampleData[chan % channelCount][readPtr];
	}

	if(++readPtr >= sampleData[0].size()) {
		readPtr = 0; //Loop
	}
}

unsigned int SamplePlayer::getLength() const {
	return sampleData.empty() ? 0u : static_cast<unsigned int>(sampleData[0].size());
}
