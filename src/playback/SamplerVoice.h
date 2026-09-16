#pragma once

#include "Program.h"
#include "QuadDispatch.h"
#include "SamplePlayer.h"
#include "SlotDispatch.h"
#include "VoiceBinding.h"
class SamplerVoice
{
public:
	void init(double sampleRate);
	void playOn(Program::Slot& slot, int velocity, size_t busChannelCount, QuadDispatch& dispatchState);
	void stop();
	void nextSamples(float* sum, size_t sumChannelCount);

	bool getIsPlaying() const { return player.getIsPlaying(); }
	MixBusIndex getBusIndex() const { return busIndex; }

	void setVoiceBinding(const VoiceBinding& binding);
	void clearVoiceBinding();
	const VoiceBinding& getVoiceBinding() const { return voiceBinding; }
	void setActiveSlot(size_t slotId);
	void clearActiveSlot();

private:
	void resolveDispatch(const Program::Slot& slot, size_t busChannelCount, QuadDispatch& dispatchState);
	void mixDryToSum(float* sum, size_t sumChannelCount);
	void mixToStereoPair(float* sum, size_t leftChannel, size_t rightChannel, float left, float right, bool isMono) const;
	void mixToMonoChannel(float* sum, size_t channel, float sample) const;

	SamplePlayer player;
	VoiceBinding voiceBinding;
	MixBusIndex busIndex = kBusMaster;

	static constexpr size_t kMaxChannels = 2;
	float gain = 1.f;
	float balance[kMaxChannels] = {1.f, 1.f};
	float dry[kMaxChannels] = {0.f, 0.f};
	SlotDispatch dispatch = SlotDispatch::Front;
	bool isMonoSample = false;
	size_t mixLeftChannel = 0;
	size_t mixRightChannel = 1;
	bool mixToAllChannels = false;

};
