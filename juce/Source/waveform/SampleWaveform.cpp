#include "SampleWaveform.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kLineWidth = 2.f;

void fillRamp(float startingValue, float increment, float* output, size_t frameCount) {
	for(size_t i = 0; i < frameCount; ++i) {
		output[i] = startingValue + increment * static_cast<float>(i);
	}
}

float maxInRange(const float* data, size_t frameCount) {
	float value = data[0];
	for(size_t i = 1; i < frameCount; ++i) {
		value = std::max(value, data[i]);
	}
	return value;
}

float minInRange(const float* data, size_t frameCount) {
	float value = data[0];
	for(size_t i = 1; i < frameCount; ++i) {
		value = std::min(value, data[i]);
	}
	return value;
}

void tableReadMono(const float* indexes, const float* table, size_t tableLength, float* output, size_t frameCount) {
	for(size_t i = 0; i < frameCount; ++i) {
		const float index = indexes[i];
		const float position = std::floor(index);
		const float fraction = index - position;

		if(position < 0.f || position >= static_cast<float>(tableLength)) {
			output[i] = 0.f;
			continue;
		}

		const size_t currentIndex = static_cast<size_t>(position);
		size_t nextIndex = currentIndex + 1;
		if(nextIndex >= tableLength) {
			nextIndex = tableLength - 1;
		}

		const float sample0 = table[currentIndex];
		const float sample1 = table[nextIndex];
		output[i] = static_cast<float>((1.0 - fraction) * sample0 + fraction * sample1);
	}
}
}

SampleWaveform::SampleWaveform() {
	setInterceptsMouseClicks(false, true);
}

SampleWaveform::~SampleWaveform() = default;

void SampleWaveform::paint(juce::Graphics& g) {
	const float midY = static_cast<float>(getHeight()) * 0.5f;

	if(waveformPath.isEmpty()) {
		const float amplitude = static_cast<float>(getHeight()) * 0.5f * amplitudeFactor;
		const float widthPerPoint = (windowEnd - windowStart) * static_cast<float>(getWidth())
			/ static_cast<float>(kResolution);
		const float minX = windowStart * static_cast<float>(getWidth());

		waveformPath.startNewSubPath(minX, midY - uiSampleData[0][0] * amplitude);
		for(size_t i = 1; i < kResolution; ++i) {
			const float x = minX + static_cast<float>(i) * widthPerPoint;
			const float y = midY - uiSampleData[0][i] * amplitude;
			waveformPath.lineTo(x, y);
		}

		if(!renderSingleLine) {
			for(int i = static_cast<int>(kResolution) - 1; i >= 0; --i) {
				waveformPath.lineTo(minX + static_cast<float>(i) * widthPerPoint,
					midY - uiSampleData[1][static_cast<size_t>(i)] * amplitude);
			}
			waveformPath.closeSubPath();
		}
	}

	g.setColour(juce::Colours::darkgrey);
	if(!renderSingleLine) {
		g.fillPath(waveformPath);
	}
	g.strokePath(waveformPath, juce::PathStrokeType(kLineWidth));

	g.reduceClipRegion(static_cast<int>(playStart * static_cast<float>(getWidth())),
		0,
		static_cast<int>(std::max((playEnd - playStart), 0.f) * static_cast<float>(getWidth())),
		getHeight());

	const float gradRatio = 0.2f;
	juce::ColourGradient gradient(juce::Colours::blue,
		0.f,
		midY - gradRatio * static_cast<float>(getHeight()),
		juce::Colours::blue,
		0.f,
		midY + gradRatio * static_cast<float>(getHeight()),
		false);
	gradient.addColour(0.5f, juce::Colours::white);
	g.setGradientFill(gradient);
	if(!renderSingleLine) {
		g.fillPath(waveformPath);
	}
	g.strokePath(waveformPath, juce::PathStrokeType(kLineWidth));
}

void SampleWaveform::resized() {
	waveformPath.clear();
	repaint();
}

void SampleWaveform::updateSampleBuf(const float* data, size_t bufLength) {
	sampleData = data;
	sampleLength = bufLength;

	processSampleData();
	waveformPath.clear();
	repaint();
}

void SampleWaveform::updateWindow(float relativeStart, float relativeEnd) {
	windowStart = relativeStart;
	windowEnd = relativeEnd;

	processSampleData();
	waveformPath.clear();
	repaint();
}

void SampleWaveform::setStart(float start) {
	playStart = start;
	waveformPath.clear();
	repaint();
}

void SampleWaveform::setEnd(float end) {
	playEnd = end;
	waveformPath.clear();
	repaint();
}

void SampleWaveform::setAmplitudeFactor(float factor) {
	amplitudeFactor = factor;
	waveformPath.clear();
	repaint();
}

void SampleWaveform::setReverse(bool isReverse) {
	reverse = isReverse;
	processSampleData();
	waveformPath.clear();
	repaint();
}

void SampleWaveform::processSampleData() {
	if(sampleData == nullptr || sampleLength == 0 || kResolution == 0) {
		return;
	}

	const float windowSize = (windowEnd - windowStart) * static_cast<float>(sampleLength)
		/ static_cast<float>(kResolution);

	if(reverse) {
		fillRamp((1.f - windowStart) * static_cast<float>(sampleLength),
			-windowSize,
			readRamp,
			kResolution);
	} else {
		fillRamp(windowStart * static_cast<float>(sampleLength), windowSize, readRamp, kResolution);
	}

	if(windowSize >= 2.f) {
		renderSingleLine = false;
		for(size_t i = 0; i < kResolution; ++i) {
			const size_t readPosition = static_cast<size_t>(std::floor(readRamp[i]));
			const size_t count = static_cast<size_t>(windowSize);
			if(readPosition + count > sampleLength) {
				uiSampleData[0][i] = 0.f;
				uiSampleData[1][i] = 0.f;
				continue;
			}

			uiSampleData[0][i] = maxInRange(sampleData + readPosition, count);
			uiSampleData[1][i] = minInRange(sampleData + readPosition, count);
		}
	} else {
		renderSingleLine = true;
		tableReadMono(readRamp, sampleData, sampleLength, uiSampleData[0], kResolution);
	}
}
