#include "HeadlessConsoleUI.h"

#include "PeakMeterDisplay.h"

#include <algorithm>
#include <curses.h>

namespace {
constexpr int kLogLineLimit = 100;
constexpr int kMeterBarWidth = 28;
constexpr int kMeterSectionRows = 1 + MixBusArray::kMasterChannelCount;
constexpr int kLogStartRow = 2 + kMeterSectionRows;

short colourPairForLevel(PeakMeterDisplay::LevelColour colour) {
	switch(colour) {
		case PeakMeterDisplay::LevelColour::Red: return 3;
		case PeakMeterDisplay::LevelColour::Yellow: return 2;
		case PeakMeterDisplay::LevelColour::Green:
		default: return 1;
	}
}
}

HeadlessConsoleUI::HeadlessConsoleUI(std::array<PreviewOutputLevels, MixBusArray::kMasterChannelCount>& inOutputLevels)
	: outputLevels(inOutputLevels) {
}

HeadlessConsoleUI::~HeadlessConsoleUI() {
	shutdown();
}

bool HeadlessConsoleUI::init() {
	if(active) {
		return true;
	}

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	timeout(0);
	curs_set(0);

	if(has_colors()) {
		start_color();
		use_default_colors();
		init_pair(1, COLOR_GREEN, -1);
		init_pair(2, COLOR_YELLOW, -1);
		init_pair(3, COLOR_RED, -1);
		init_pair(4, COLOR_CYAN, -1);
	}

	active = true;
	drawFrame();
	return true;
}

void HeadlessConsoleUI::shutdown() {
	if(!active) {
		return;
	}

	endwin();
	active = false;
}

void HeadlessConsoleUI::appendLog(const std::string& line) {
	const std::string trimmed = trimTrailingNewlines(line);
	if(trimmed.empty()) {
		return;
	}

	std::lock_guard<std::mutex> lock(logMutex);
	logLines.push_back(trimmed);
	while(logLines.size() > static_cast<size_t>(kLogLineLimit)) {
		logLines.pop_front();
	}
}

void HeadlessConsoleUI::setProgramName(const std::string& inProgramName) {
	programName = inProgramName;
}

bool HeadlessConsoleUI::pollAndDraw(int timeoutMs, std::string& submittedCommand) {
	if(!active) {
		return false;
	}

	timeout(timeoutMs);
	const int key = getch();
	submittedCommand.clear();

	if(key == ERR) {
		drawFrame();
		return false;
	}

	if(key == 27 || key == 'q' || key == 'Q') {
		submittedCommand = "quit";
		drawFrame();
		return true;
	}

	if(key == 'r' || key == 'R') {
		submittedCommand = "reload";
		drawFrame();
		return true;
	}

	if(key == '\n' || key == KEY_ENTER) {
		submittedCommand = inputBuffer;
		inputBuffer.clear();
		drawFrame();
		return !submittedCommand.empty();
	}

	if(key == KEY_BACKSPACE || key == 127 || key == 8) {
		if(!inputBuffer.empty()) {
			inputBuffer.pop_back();
		}
	} else if(key == 21) {
		inputBuffer.clear();
	} else if(key >= 32 && key <= 126) {
		inputBuffer.push_back(static_cast<char>(key));
	}

	drawFrame();
	return false;
}

void HeadlessConsoleUI::drawChannelMeter(int row, int channelIndex, float rmsLevel, float peakHoldLevel) const {
	const float normalisedRms = PeakMeterDisplay::rmsToNormalised(rmsLevel);
	const float normalisedPeak = PeakMeterDisplay::peakToNormalised(peakHoldLevel);
	const int rmsFill = static_cast<int>(std::lround(normalisedRms * static_cast<float>(kMeterBarWidth)));
	const int peakMark = static_cast<int>(std::lround(normalisedPeak * static_cast<float>(kMeterBarWidth)));

	mvprintw(row, 0, "%d %5s [", channelIndex, PeakMeterDisplay::formatPeakDb(peakHoldLevel).c_str());

	for(int i = 0; i < kMeterBarWidth; ++i) {
		const bool showPeakHold = normalisedPeak > 0.02f && i == peakMark;
		if(showPeakHold) {
			attron(A_BOLD);
			addch('|');
			attroff(A_BOLD);
			continue;
		}

		if(i < rmsFill) {
			if(has_colors()) {
				const short colourPair = colourPairForLevel(PeakMeterDisplay::colourForLevel(normalisedRms));
				attron(COLOR_PAIR(colourPair));
				addch('#');
				attroff(COLOR_PAIR(colourPair));
			} else {
				addch('#');
			}
		} else {
			addch('-');
		}
	}

	addch(']');
}

void HeadlessConsoleUI::drawFrame() {
	for(size_t channel = 0; channel < outputLevels.size(); ++channel) {
		float unusedRms = 0.f;
		float unusedPeak = 0.f;
		outputLevels[channel].fetchLevels(rms[channel], unusedRms, peakHold[channel], unusedPeak);
	}

	erase();
	attron(A_BOLD);
	mvprintw(0, 0, "%s", programName.c_str());
	attroff(A_BOLD);

	mvprintw(2, 0, "Outputs");
	for(size_t channel = 0; channel < outputLevels.size(); ++channel) {
		drawChannelMeter(static_cast<int>(3 + channel), static_cast<int>(channel), rms[channel], peakHold[channel]);
	}

	mvprintw(kLogStartRow, 0, "Log");
	int logRow = kLogStartRow + 1;
	int maxRows = 0;
	int maxCols = 0;
	getmaxyx(stdscr, maxRows, maxCols);

	{
		std::lock_guard<std::mutex> lock(logMutex);
		const int availableRows = std::max(0, maxRows - kLogStartRow - 3);
		const int startIndex = std::max(0, static_cast<int>(logLines.size()) - availableRows);

		for(int i = startIndex; i < static_cast<int>(logLines.size()); ++i) {
			if(logRow >= maxRows - 3) {
				break;
			}

			const std::string& line = logLines[static_cast<size_t>(i)];
			mvprintw(logRow, 2, "%.*s", std::max(0, maxCols - 3), line.c_str());
			++logRow;
		}
	}

	mvprintw(maxRows - 2, 0, "Command: %s", inputBuffer.c_str());
	if(has_colors()) {
		attron(COLOR_PAIR(4));
	}
	mvprintw(maxRows - 1, 0, "[Enter] reload|quit   [Esc] quit   shortcuts: r=reload q=quit");
	if(has_colors()) {
		attroff(COLOR_PAIR(4));
	}

	refresh();
}

std::string HeadlessConsoleUI::trimTrailingNewlines(const std::string& text) {
	std::string trimmed = text;
	while(!trimmed.empty() && (trimmed.back() == '\n' || trimmed.back() == '\r')) {
		trimmed.pop_back();
	}
	return trimmed;
}
