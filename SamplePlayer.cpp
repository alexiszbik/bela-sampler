#include "SamplePlayer.h"

#include <cmath>

static const double kGrainDurationSec = 0.08;
static const double twoPi = 2.0 * M_PI;

void SamplePlayer::init(double inPlayingSampleRate) {
	playingSampleRate = inPlayingSampleRate;
	grainOutputLength = playingSampleRate * kGrainDurationSec;
	grainPhaseIncrement = 1.0 / grainOutputLength;
	grainHop = grainOutputLength * 0.5;
	updateSrSpeed();
}

void SamplePlayer::setSample(const Sample* newSample) {
	sample = newSample;
	updateSrSpeed();
	resetPlaybackState();
}

void SamplePlayer::setPlayMode(EPlayerMode pm) {
	if(playMode == pm) {
		return;
	}

	playMode = pm;
	resetPlaybackState();
}

void SamplePlayer::updateSrSpeed() {
	if(sample == nullptr) {
		srSpeed = 1.0;
		return;
	}

	srSpeed = static_cast<double>(sample->getSampleRate()) / playingSampleRate;
}

void SamplePlayer::resetPlaybackState() {
	readPos = 0.0;
	speed = 1.f;
	granularSpeed = 1.f;
	granularPitch = 1.f;
	resetGranularState();
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

float SamplePlayer::hannEnvelope(double phase) const {
	return static_cast<float>(0.5 * (1.0 - std::cos(twoPi * phase)));
}

void SamplePlayer::wrapSampleIndex(double& index) const {
	if(sample == nullptr) {
		return;
	}

	const double length = sample->getSampleLengthDouble();
	while(index >= length) {
		index -= length;
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
	sample->tableRead(grain.readPos, grainOut, bufSize);

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

	sample->tableRead(readPos, buf, bufSize);

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
	if(sample == nullptr || sample->getLength() == 0) {
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
