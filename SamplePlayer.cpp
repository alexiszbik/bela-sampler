#include "SamplePlayer.h"

#include <libraries/AudioFile/AudioFile.h>
#include <libraries/sndfile/sndfile.h>

#include <cmath>

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
	readPos = 0.0;
	speed = 1.f;
	return true;
}

void SamplePlayer::tableRead(double index, size_t tableLength, float* buf, size_t bufSize) {
	const double p = index;
	const double q = std::floor(p);
	const double r = p - q;

	int nextIndex = static_cast<int>(q) + 1;
	if(nextIndex >= static_cast<int>(tableLength)) {
		nextIndex = 0;
	} else if(nextIndex < 0) {
		nextIndex = static_cast<int>(tableLength) - 1;
	}

	const int currentIndex = static_cast<int>(q);

	for(size_t channel = 0; channel < bufSize; channel++) {
		const unsigned int srcChan = channel % channelCount;
		const float sample0 = sampleData[srcChan][currentIndex];
		const float sample1 = sampleData[srcChan][nextIndex];
		buf[channel] += static_cast<float>((1.0 - r) * sample0 + r * sample1);
	}
}

void SamplePlayer::nextSamples(float* buf, size_t bufSize) {
	if(sampleData.empty() || sampleData[0].empty()) {
		return;
	}

	if(speed <= 0.f) {
		return;
	}

	const size_t length = sampleData[0].size();
	const double srSpeed = static_cast<double>(sampleRate) / playingSampleRate;
	const double playbackSpeed = static_cast<double>(speed) * srSpeed;

	tableRead(readPos, length, buf, bufSize);

	readPos += playbackSpeed;
	if(readPos >= static_cast<double>(length)) {
		readPos = std::fmod(readPos, static_cast<double>(length));
	}
}

unsigned int SamplePlayer::getLength() const {
	return sampleData.empty() ? 0u : static_cast<unsigned int>(sampleData[0].size());
}

size_t SamplePlayer::getRamBytes() const {
	if(sampleData.empty() || sampleData[0].empty()) {
		return 0;
	}

	return sampleData[0].size() * channelCount * sizeof(float);
}
