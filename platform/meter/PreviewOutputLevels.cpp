#include "PreviewOutputLevels.h"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace {
constexpr float kDisplayDecay = 0.82f;
constexpr float kHoldDecay = 0.82f;
constexpr double kPeakHoldDurationSec = 1.0;

using SteadyClock = std::chrono::steady_clock;

void updatePeakHold(float blockPeak,
	float& holdPeak,
	SteadyClock::time_point& holdSetTime) {
	const auto now = SteadyClock::now();

	if(blockPeak >= holdPeak) {
		holdPeak = blockPeak;
		holdSetTime = now;
		return;
	}

	const double elapsedSec = std::chrono::duration<double>(now - holdSetTime).count();
	if(elapsedSec < kPeakHoldDurationSec) {
		return;
	}

	holdPeak *= kHoldDecay;
}
}

void PreviewOutputLevels::updateAtomicMax(std::atomic<float>& target, float value) {
	float current = target.load(std::memory_order_relaxed);
	while(value > current
		&& !target.compare_exchange_weak(current, value, std::memory_order_relaxed)) {
	}
}

float PreviewOutputLevels::computeRms(const float* channelData, int numSamples) {
	if(channelData == nullptr || numSamples <= 0) {
		return 0.f;
	}

	double sumSquares = 0.0;
	for(int i = 0; i < numSamples; ++i) {
		const double sample = static_cast<double>(channelData[i]);
		sumSquares += sample * sample;
	}

	return static_cast<float>(std::sqrt(sumSquares / static_cast<double>(numSamples)));
}

float PreviewOutputLevels::computePeak(const float* channelData, int numSamples) {
	if(channelData == nullptr || numSamples <= 0) {
		return 0.f;
	}

	float peak = 0.f;
	for(int i = 0; i < numSamples; ++i) {
		peak = std::max(peak, std::abs(channelData[i]));
	}

	return peak;
}

void PreviewOutputLevels::processBlock(const float* const* output,
	int numChannels,
	int numSamples) {
	if(output == nullptr || numSamples <= 0) {
		return;
	}

	const float* leftChannel = output[0];
	const float* rightChannel = (numChannels > 1 && output[1] != nullptr) ? output[1] : leftChannel;

	const float rmsL = computeRms(leftChannel, numSamples);
	const float rmsR = computeRms(rightChannel, numSamples);
	const float peakL = computePeak(leftChannel, numSamples);
	const float peakR = computePeak(rightChannel, numSamples);

	updateAtomicMax(blockRmsL, rmsL);
	updateAtomicMax(blockRmsR, rmsR);
	updateAtomicMax(blockPeakL, peakL);
	updateAtomicMax(blockPeakR, peakR);
}

void PreviewOutputLevels::fetchLevels(float& rmsL,
	float& rmsR,
	float& peakHoldL,
	float& peakHoldR) {
	const float blockRmsLeft = blockRmsL.exchange(0.f, std::memory_order_relaxed);
	const float blockRmsRight = blockRmsR.exchange(0.f, std::memory_order_relaxed);
	const float blockPeakLeft = blockPeakL.exchange(0.f, std::memory_order_relaxed);
	const float blockPeakRight = blockPeakR.exchange(0.f, std::memory_order_relaxed);

	displayRmsL = std::max(blockRmsLeft, displayRmsL * kDisplayDecay);
	displayRmsR = std::max(blockRmsRight, displayRmsR * kDisplayDecay);

	updatePeakHold(blockPeakLeft, holdPeakL, holdPeakSetTimeL);
	updatePeakHold(blockPeakRight, holdPeakR, holdPeakSetTimeR);

	rmsL = displayRmsL;
	rmsR = displayRmsR;
	peakHoldL = holdPeakL;
	peakHoldR = holdPeakR;
}
