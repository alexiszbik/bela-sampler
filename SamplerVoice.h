#pragma once

#include "Program.h"
#include "SamplePlayer.h"
#include "VoiceBinding.h"

class SamplerVoice
{
public:
	void init(double sampleRate);
	void playOn(const Program::Slot& slot, int velocity);
	void stop();
	void nextSamples(float* sum, size_t channelCount);

	bool getIsPlaying() const { return player.getIsPlaying(); }
	size_t getBusIndex() const { return busIndex; }

	void setVoiceBinding(const VoiceBinding& binding);
	void clearVoiceBinding();
	const VoiceBinding& getVoiceBinding() const { return voiceBinding; }
	void setActiveSlot(size_t slotId);
	void clearActiveSlot();

private:
	void mixDryToSum(float* sum, size_t channelCount);

	SamplePlayer player;
	VoiceBinding voiceBinding;
	size_t busIndex = 0;

	static constexpr size_t kMaxChannels = 2;
	float gain = 1.f;
	float dry[kMaxChannels] = {0.f, 0.f};
};
