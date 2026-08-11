#include "LogWindow.h"

#include "LogBuffer.h"

namespace {
constexpr int kRefreshHz = 10;
constexpr size_t kMaxDrainBytes = 1024;
constexpr int kMaxDisplayedChars = 24 * 1024;
}

LogWindow::LogWindow() {
	logEditor.setMultiLine(true);
	logEditor.setReadOnly(true);
	logEditor.setScrollbarsShown(true);
	logEditor.setCaretVisible(false);
	logEditor.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 13.f, juce::Font::plain));
	addAndMakeVisible(logEditor);

	startTimerHz(kRefreshHz);
}

void LogWindow::paint(juce::Graphics& g) {
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void LogWindow::resized() {
	logEditor.setBounds(getLocalBounds().reduced(8));
}

void LogWindow::trimDisplayedText() {
	const int length = logEditor.getText().length();
	if(length <= kMaxDisplayedChars) {
		return;
	}

	logEditor.setText(logEditor.getText().substring(length - kMaxDisplayedChars));
	logEditor.moveCaretToEnd();
}

void LogWindow::timerCallback() {
	char drained[kMaxDrainBytes];
	const size_t drainedBytes = LogBuffer::getInstance().drainTo(drained, sizeof(drained));
	if(drainedBytes == 0) {
		return;
	}

	logEditor.moveCaretToEnd();
	logEditor.insertTextAtCaret(juce::String::fromUTF8(drained, static_cast<int>(drainedBytes)));
	trimDisplayedText();
}
