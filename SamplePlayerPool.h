#pragma once

#include "Program.h"
#include "SamplerVoice.h"

#include <vector>

class SamplePlayerPool
{
public:
	void init(double sampleRate, size_t count);
	void playOn(SamplerVoice* voice, const Program::Slot& slot, int velocity);
	void stop(SamplerVoice* voice);
	void nextSamples(float* buf, size_t bufSize);

	size_t getCount() const { return voices.size(); }

	using VoiceIterator = std::vector<SamplerVoice>::iterator;
	using VoiceConstIterator = std::vector<SamplerVoice>::const_iterator;

	VoiceIterator begin() { return voices.begin(); }
	VoiceIterator end() { return voices.end(); }
	VoiceConstIterator begin() const { return voices.begin(); }
	VoiceConstIterator end() const { return voices.end(); }

private:
	std::vector<SamplerVoice> voices;
};
