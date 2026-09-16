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

void buildEnvelopePath(juce::Path& path,
	float centerY,
	float amplitude,
	WaveformBuffer<float>& maxSamples,
	WaveformBuffer<float>& minSamples,
	float minX,
	float widthPerPoint,
	size_t resolution) {
	path.startNewSubPath(minX, centerY - maxSamples[0] * amplitude);
	for(size_t i = 1; i < resolution; ++i) {
		const float x = minX + static_cast<float>(i) * widthPerPoint;
		const float y = centerY - maxSamples[i] * amplitude;
		path.lineTo(x, y);
	}

	for(int i = static_cast<int>(resolution) - 1; i >= 0; --i) {
		path.lineTo(minX + static_cast<float>(i) * widthPerPoint,
			centerY - minSamples[static_cast<size_t>(i)] * amplitude);
	}

	path.closeSubPath();
}

void buildLinePath(juce::Path& path,
	float centerY,
	float amplitude,
	WaveformBuffer<float>& samples,
	float minX,
	float widthPerPoint,
	size_t resolution) {
	path.startNewSubPath(minX, centerY - samples[0] * amplitude);
	for(size_t i = 1; i < resolution; ++i) {
		const float x = minX + static_cast<float>(i) * widthPerPoint;
		const float y = centerY - samples[i] * amplitude;
		path.lineTo(x, y);
	}
}
}

SampleWaveform::SampleWaveform() {
	setInterceptsMouseClicks(false, true);
}

SampleWaveform::~SampleWaveform() = default;

void SampleWaveform::clearWaveformPaths() {
	waveformPaths[0].clear();
	waveformPaths[1].clear();
}

void SampleWaveform::buildWaveformPaths() {
	clearWaveformPaths();

	const float height = static_cast<float>(getHeight());
	const float widthPerPoint = (windowEnd - windowStart) * static_cast<float>(getWidth())
		/ static_cast<float>(kResolution);
	const float minX = windowStart * static_cast<float>(getWidth());

	const size_t channelCount = isStereoDisplay ? 2u : 1u;
	const float laneHeight = height / static_cast<float>(channelCount);

	for(size_t channel = 0; channel < channelCount; ++channel) {
		const float centerY = laneHeight * (static_cast<float>(channel) + 0.5f);
		const float amplitude = laneHeight * 0.5f * amplitudeFactor;

		if(renderSingleLine) {
			buildLinePath(waveformPaths[channel],
				centerY,
				amplitude,
				uiSampleMax[channel],
				minX,
				widthPerPoint,
				kResolution);
		} else {
			buildEnvelopePath(waveformPaths[channel],
				centerY,
				amplitude,
				uiSampleMax[channel],
				uiSampleMin[channel],
				minX,
				widthPerPoint,
				kResolution);
		}
	}
}

void SampleWaveform::paint(juce::Graphics& g) {
	if(waveformPaths[0].isEmpty()) {
		buildWaveformPaths();
	}

	const float height = static_cast<float>(getHeight());
	const size_t channelCount = isStereoDisplay ? 2u : 1u;
	const float laneHeight = height / static_cast<float>(channelCount);

	g.setColour(juce::Colours::darkgrey);
	for(size_t channel = 0; channel < channelCount; ++channel) {
		if(!renderSingleLine) {
			g.fillPath(waveformPaths[channel]);
		}
		g.strokePath(waveformPaths[channel], juce::PathStrokeType(kLineWidth));
	}

	if(isStereoDisplay) {
		g.setColour(juce::Colour(0xff505050));
		g.drawHorizontalLine(static_cast<int>(laneHeight), 0.f, static_cast<float>(getWidth()));
	}

	g.reduceClipRegion(static_cast<int>(playStart * static_cast<float>(getWidth())),
		0,
		static_cast<int>(std::max((playEnd - playStart), 0.f) * static_cast<float>(getWidth())),
		getHeight());

	for(size_t channel = 0; channel < channelCount; ++channel) {
		const float centerY = laneHeight * (static_cast<float>(channel) + 0.5f);
		const float gradRatio = 0.2f;
		juce::ColourGradient gradient(juce::Colours::blue,
			0.f,
			centerY - gradRatio * laneHeight,
			juce::Colours::blue,
			0.f,
			centerY + gradRatio * laneHeight,
			false);
		gradient.addColour(0.5f, juce::Colours::white);
		g.setGradientFill(gradient);

		if(!renderSingleLine) {
			g.fillPath(waveformPaths[channel]);
		}
		g.strokePath(waveformPaths[channel], juce::PathStrokeType(kLineWidth));
	}
}

void SampleWaveform::resized() {
	clearWaveformPaths();
	repaint();
}

void SampleWaveform::updateSampleBuf(const float* left, size_t bufLength, const float* right) {
	sampleDataLeft = left;
	sampleDataRight = right;
	sampleLength = bufLength;
	isStereoDisplay = right != nullptr;

	processSampleData();
	clearWaveformPaths();
	repaint();
}

void SampleWaveform::updateWindow(float relativeStart, float relativeEnd) {
	windowStart = relativeStart;
	windowEnd = relativeEnd;

	processSampleData();
	clearWaveformPaths();
	repaint();
}

void SampleWaveform::setStart(float start) {
	playStart = start;
	clearWaveformPaths();
	repaint();
}

void SampleWaveform::setEnd(float end) {
	playEnd = end;
	clearWaveformPaths();
	repaint();
}

void SampleWaveform::setAmplitudeFactor(float factor) {
	amplitudeFactor = factor;
	clearWaveformPaths();
	repaint();
}

void SampleWaveform::setReverse(bool isReverse) {
	reverse = isReverse;
	processSampleData();
	clearWaveformPaths();
	repaint();
}

void SampleWaveform::processSampleData() {
	if(sampleDataLeft == nullptr || sampleLength == 0 || kResolution == 0) {
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

	const size_t channelCount = isStereoDisplay ? 2u : 1u;
	const float* channelData[2] = {sampleDataLeft, sampleDataRight};

	if(windowSize >= 2.f) {
		renderSingleLine = false;

		for(size_t channel = 0; channel < channelCount; ++channel) {
			const float* data = channelData[channel];
			if(data == nullptr) {
				continue;
			}

			for(size_t i = 0; i < kResolution; ++i) {
				const size_t readPosition = static_cast<size_t>(std::floor(readRamp[i]));
				const size_t count = static_cast<size_t>(windowSize);
				if(readPosition + count > sampleLength) {
					uiSampleMax[channel][i] = 0.f;
					uiSampleMin[channel][i] = 0.f;
					continue;
				}

				uiSampleMax[channel][i] = maxInRange(data + readPosition, count);
				uiSampleMin[channel][i] = minInRange(data + readPosition, count);
			}
		}
	} else {
		renderSingleLine = true;

		for(size_t channel = 0; channel < channelCount; ++channel) {
			const float* data = channelData[channel];
			if(data == nullptr) {
				continue;
			}

			tableReadMono(readRamp, data, sampleLength, uiSampleMax[channel], kResolution);
		}
	}
}
