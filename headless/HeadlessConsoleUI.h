#pragma once

#include "MixBusArray.h"
#include "PreviewOutputLevels.h"

#include <array>
#include <deque>
#include <mutex>
#include <string>

class HeadlessConsoleUI
{
public:
	explicit HeadlessConsoleUI(std::array<PreviewOutputLevels, MixBusArray::kMasterChannelCount>& outputLevels);
	~HeadlessConsoleUI();

	bool init();
	void shutdown();

	void appendLog(const std::string& line);
	void setProgramName(const std::string& programName);
	bool pollAndDraw(int timeoutMs, std::string& submittedCommand);

private:
	void drawFrame();
	void drawChannelMeter(int row, int channelIndex, float rms, float peakHold) const;
	static std::string trimTrailingNewlines(const std::string& text);

	std::array<PreviewOutputLevels, MixBusArray::kMasterChannelCount>& outputLevels;
	std::array<float, MixBusArray::kMasterChannelCount> rms {};
	std::array<float, MixBusArray::kMasterChannelCount> peakHold {};
	std::mutex logMutex;
	std::deque<std::string> logLines;
	std::string programName;
	std::string inputBuffer;
	bool active = false;
};
