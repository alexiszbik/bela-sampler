#include "ReverbEffect.h"

void ReverbEffect::init(int channelCount, double sampleRate) {
	const float fsr = static_cast<float>(sampleRate);

	reverb.init(channelCount, fsr);
	lfo.init(sampleRate);
	lfo.setFrequency(0.5f);
}

void ReverbEffect::setSendLevel(float value) {
	sendLevel.setValue(value * value);
}

void ReverbEffect::process(float* sum, size_t channelCount, bool enabled) {
	if(!enabled || sum == nullptr || channelCount == 0) {
		return;
	}

	const size_t frameCount = 1;
	const float send = sendLevel.getAndStep();

	float lfoValue = lfo.process() * 0.25f;

	reverb.setRoomSize(50.f, &lfoValue, frameCount);

	float inLeft = sum[0] * send;
	float inRight = channelCount > 1 ? sum[1] : sum[0];
	inRight *= send;

	reverb.process(&inLeft, &inRight, 1, reverbTime);

	sum[0] += inLeft;
	if(channelCount > 1) {
		sum[1] += inRight;
	}
}
