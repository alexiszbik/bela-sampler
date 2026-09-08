#pragma once

#include <atomic>
#include <chrono>

class PreviewOutputLevels
{
public:
	void processBlock(const float* const* output, int numChannels, int numSamples);
	void fetchLevels(float& rmsL, float& rmsR, float& peakHoldL, float& peakHoldR);

private:
	using SteadyClock = std::chrono::steady_clock;

	static void updateAtomicMax(std::atomic<float>& target, float value);
	static float computeRms(const float* channelData, int numSamples);
	static float computePeak(const float* channelData, int numSamples);

	std::atomic<float> blockRmsL {0.f};
	std::atomic<float> blockRmsR {0.f};
	std::atomic<float> blockPeakL {0.f};
	std::atomic<float> blockPeakR {0.f};

	float displayRmsL = 0.f;
	float displayRmsR = 0.f;
	float holdPeakL = 0.f;
	float holdPeakR = 0.f;
	SteadyClock::time_point holdPeakSetTimeL {};
	SteadyClock::time_point holdPeakSetTimeR {};
};
