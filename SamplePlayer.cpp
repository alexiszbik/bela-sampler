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
	isPlaying = false;
}

void SamplePlayer::trigger() {
	if(isReversed && sample != nullptr && sample->getLength() > 0) {
		readPos = sample->getSampleLengthDouble() - 1.0;
		sequentialPos = readPos;
	} else {
		readPos = 0.0;
		resetGranularState();
	}

	isPlaying = true;
}

void SamplePlayer::stop() {
	isPlaying = false;
	isLoop = false;
	clearActiveSlot();
}

void SamplePlayer::setVoiceBinding(const VoiceBinding& binding) {
	voiceBinding = binding;
}

void SamplePlayer::clearVoiceBinding() {
	voiceBinding = VoiceBinding{};
}

void SamplePlayer::setActiveSlot(size_t slotId) {
	voiceBinding.activeSlotId = slotId;
}

void SamplePlayer::clearActiveSlot() {
	voiceBinding.activeSlotId = VoiceBinding::kInvalidSlot;
}

void SamplePlayer::setPlayMode(EPlayerMode pm) {
	if(playMode == pm) {
		return;
	}

	playMode = pm;
	readPos = 0.0;
	resetGranularState();
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
	isReversed = false;
	isPlaying = false;
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
	if(isLoop) {
		if(isReversed) {
			while(index < 0.0) {
				index += length;
			}
		} else {
			while(index >= length) {
				index -= length;
			}
		}
	} else if(isReversed) {
		if(index < 0.0) {
			index = 0.0;
		}
	} else if(index >= length) {
		index = length;
	}
}

void SamplePlayer::spawnGrain(Grain& grain) {
	const double length = sample->getSampleLengthDouble();
	if(!isLoop && ((isReversed && sequentialPos <= 0.0) || (!isReversed && sequentialPos >= length))) {
		return;
	}

	grain.readPos = sequentialPos;
	grain.phase = 0.0;

	if(isReversed) {
		sequentialPos -= grainHop * srSpeed;
	} else {
		sequentialPos += grainHop * srSpeed;
	}

	wrapSampleIndex(sequentialPos);
}

void SamplePlayer::processGrain(Grain& grain, float* buf, size_t bufSize, double grainReadSpeed) {
	if(grain.phase >= 1.0) {
		return;
	}

	if(!isLoop && ((isReversed && grain.readPos <= 0.0) || (!isReversed && grain.readPos >= sample->getSampleLengthDouble()))) {
		grain.phase = 1.0;
		return;
	}

	float grainOut[2] = {0.f, 0.f};
	sample->tableRead(grain.readPos, grainOut, bufSize, isLoop);

	const float env = hannEnvelope(grain.phase);
	for(size_t channel = 0; channel < bufSize; channel++) {
		buf[channel] += grainOut[channel] * env;
	}

	const double direction = isReversed ? -1.0 : 1.0;
	grain.readPos += grainReadSpeed * direction;
	wrapSampleIndex(grain.readPos);

	grain.phase += grainPhaseIncrement;
}

void SamplePlayer::nextSamplesNormal(float* buf, size_t bufSize) {
	const double length = sample->getSampleLengthDouble();
	if(!isLoop && ((isReversed && readPos <= 0.0) || (!isReversed && readPos >= length))) {
		isPlaying = false;
		return;
	}

	const double playbackSpeed = static_cast<double>(speed) * srSpeed;
	const double direction = isReversed ? -1.0 : 1.0;

	sample->tableRead(readPos, buf, bufSize, isLoop);

	readPos += playbackSpeed * direction;
	wrapSampleIndex(readPos);

	if(!isLoop && ((isReversed && readPos <= 0.0) || (!isReversed && readPos >= length))) {
		isPlaying = false;
	}
}

void SamplePlayer::nextSamplesGranular(float* buf, size_t bufSize) {
	if(granularSpeed <= 0.f || granularPitch <= 0.f) {
		return;
	}

	if(!isLoop && ((isReversed && sequentialPos <= 0.0) || (!isReversed && sequentialPos >= sample->getSampleLengthDouble()))) {
		bool hasActiveGrain = false;
		for(const Grain& grain : grains) {
			if(grain.phase < 1.0) {
				hasActiveGrain = true;
				break;
			}
		}
		if(!hasActiveGrain) {
			isPlaying = false;
			return;
		}
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
	if(!isPlaying || sample == nullptr || sample->getLength() == 0) {
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
