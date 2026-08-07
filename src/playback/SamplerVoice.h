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
	void nextSamples(float* sum, size_t sumChannelCount, size_t playerOutputChannels);

	bool getIsPlaying() const { return player.getIsPlaying(); }
	MixBusIndex getBusIndex() const { return busIndex; }

	void setVoiceBinding(const VoiceBinding& binding);
	void clearVoiceBinding();
	const VoiceBinding& getVoiceBinding() const { return voiceBinding; }
	void setActiveSlot(size_t slotId);
	void clearActiveSlot();

private:
	void mixDryToSum(float* sum, size_t sumChannelCount);

	SamplePlayer player;
	VoiceBinding voiceBinding;
	MixBusIndex busIndex = kBusMaster;

	static constexpr size_t kMaxChannels = 2;
	float gain = 1.f;
	float dry[kMaxChannels] = {0.f, 0.f};
};
