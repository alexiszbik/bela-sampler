#include "SamplePlayer.h"

#include <libraries/AudioFile/AudioFile.h>
#include <libraries/sndfile/sndfile.h>

#include <cmath>

static const double kGrainDurationSec = 0.08;
static const double twoPi = 2.0 * M_PI;

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
	granularSpeed = 1.f;
	granularPitch = 1.f;
	sampleLength = sampleData[0].size();
	dSampleLength =  static_cast<double>(sampleLength);
	srSpeed = static_cast<double>(sampleRate) / playingSampleRate;

	grainOutputLength = playingSampleRate * kGrainDurationSec;
	grainPhaseIncrement = 1.0 / grainOutputLength;
	grainHop = grainOutputLength * 0.5;
	resetGranularState();

	return true;
}

void SamplePlayer::resetGranularState() {
	sequentialPos = 0.0;
	grainHopCounter = 0.0;
	nextGrainIndex = 0;
	for(Grain& grain : grains) {
		grain.readPos = 0.0;
		grain.phase = 1.0;
	}
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

float SamplePlayer::hannEnvelope(double phase) const {
	return static_cast<float>(0.5 * (1.0 - std::cos(twoPi * phase)));
}

void SamplePlayer::wrapSampleIndex(double& index) const {
	while(index >= dSampleLength) {
		index -= dSampleLength;
	}
}

void SamplePlayer::spawnGrain(Grain& grain) {
	grain.readPos = sequentialPos;
	grain.phase = 0.0;

	sequentialPos += grainHop * srSpeed;
	wrapSampleIndex(sequentialPos);
}

void SamplePlayer::processGrain(Grain& grain, float* buf, size_t bufSize, double grainReadSpeed) {
	if(grain.phase >= 1.0) {
		return;
	}

	float grainOut[2] = {0.f, 0.f};
	tableRead(grain.readPos, sampleLength, grainOut, bufSize);

	const float env = hannEnvelope(grain.phase);
	for(size_t channel = 0; channel < bufSize; channel++) {
		buf[channel] += grainOut[channel] * env;
	}

	grain.readPos += grainReadSpeed;
	wrapSampleIndex(grain.readPos);

	grain.phase += grainPhaseIncrement;
}

void SamplePlayer::nextSamplesNormal(float* buf, size_t bufSize) {
	const double playbackSpeed = static_cast<double>(speed) * srSpeed;

	tableRead(readPos, sampleLength, buf, bufSize);

	readPos += playbackSpeed;
	wrapSampleIndex(readPos);
}

void SamplePlayer::nextSamplesGranular(float* buf, size_t bufSize) {
	if(granularSpeed <= 0.f || granularPitch <= 0.f) {
		return;
	}

	const double grainReadSpeed = srSpeed * static_cast<double>(granularPitch);
	const double hop = grainHop / static_cast<double>(granularSpeed);

	if(grainHopCounter <= 0.0) {
		spawnGrain(grains[nextGrainIndex]);
		nextGrainIndex = 1 - nextGrainIndex;
		grainHopCounter = hop;
	}

	for(Grain& grain : grains) {
		processGrain(grain, buf, bufSize, grainReadSpeed);
	}

	grainHopCounter -= 1.0;
}

void SamplePlayer::nextSamples(float* buf, size_t bufSize) {
	if(sampleLength == 0) {
		return;
	}

	if(playMode == Normal) {
		if(speed <= 0.f) {
			return;
		}
		nextSamplesNormal(buf, bufSize);
	} else {
		nextSamplesGranular(buf, bufSize);
	}
}

size_t SamplePlayer::getRamBytes() const {
	return sampleLength * channelCount * sizeof(float);
}
