#pragma once

#include "MixBusArray.h"
#include "Program.h"
#include "QuadDispatch.h"
#include "SamplerVoice.h"

#include <cstddef>
#include <vector>

class SamplePlayerPool
{
public:
	void init(double sampleRate, size_t count);
	void playOn(SamplerVoice* voice, Program::Slot& slot, int velocity, size_t busChannelCount, QuadDispatch& dispatchState);
	void stop(SamplerVoice* voice);
	void nextSamples(MixBusArray& mixBuses);

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
