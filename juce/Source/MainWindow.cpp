#include "MainWindow.h"

MainWindow::MainWindow(juce::String name)
	: DocumentWindow(name,
		juce::Desktop::getInstance().getDefaultLookAndFeel()
			.findColour(juce::ResizableWindow::backgroundColourId),
		DocumentWindow::allButtons),
	  rootComponent(previewHost) {
	setUsingNativeTitleBar(true);

	previewHost.initialise();

	setContentNonOwned(&rootComponent, true);
	setResizable(true, true);
	centreWithSize(1000, 700);
	setVisible(true);
}

MainWindow::~MainWindow() {
	previewHost.shutdown();
}

void MainWindow::closeButtonPressed() {
	juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
