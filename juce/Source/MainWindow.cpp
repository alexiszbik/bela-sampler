#include "MainWindow.h"

MainWindow::MainWindow(juce::String name, SamplerAudioEngine& audioEngine)
	: DocumentWindow(name,
		juce::Desktop::getInstance().getDefaultLookAndFeel()
			.findColour(juce::ResizableWindow::backgroundColourId),
		DocumentWindow::allButtons),
	  engine(audioEngine) {
	setUsingNativeTitleBar(true);
	setContentNonOwned(&logWindow, true);
	setResizable(true, true);
	centreWithSize(900, 600);
	setVisible(true);

	setName("Bela Sampler Desktop - "
		+ juce::String(static_cast<int>(engine.getSamples().size())) + " samples, "
		+ juce::String(static_cast<int>(engine.getProgramBank().getLoadedProgramCount())) + " programs");
}

void MainWindow::closeButtonPressed() {
	juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
