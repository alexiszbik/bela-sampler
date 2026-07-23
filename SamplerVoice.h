#pragma once

#include "BiquadFilter.h"
#include "Program.h"
#include "SamplePlayer.h"
#include "VoiceBinding.h"

class SamplerVoice
{
public:
	void init(double sampleRate);
	void playOn(const Program::Slot& slot, int velocity);
	void stop();
	void nextSamples(float* mixBus, size_t mixChannelCount);

	bool getIsPlaying() const { return player.getIsPlaying(); }

	void setVoiceBinding(const VoiceBinding& binding);
	void clearVoiceBinding();
	const VoiceBinding& getVoiceBinding() const { return voiceBinding; }
	void setActiveSlot(size_t slotId);
	void clearActiveSlot();

private:
	void configureFilters();
	void resetFilters();
	void mixToBus(float* mixBus, size_t mixChannelCount);

	SamplePlayer player;
	VoiceBinding voiceBinding;
	double playingSampleRate = 44100.0;

	static constexpr size_t kMaxFilterChannels = 2;
	BiquadFilter filters[kMaxFilterChannels];
	size_t filterCount = 1;
	float gain = 1.f;
	float dry[kMaxFilterChannels] = {0.f, 0.f};
};
