#include "MainWindow.h"
#include "SamplerDesktopPaths.h"
#include "SamplerLog.h"

MainWindow::MainWindow(juce::String name)
	: DocumentWindow(name,
		juce::Desktop::getInstance().getDefaultLookAndFeel()
			.findColour(juce::ResizableWindow::backgroundColourId),
		DocumentWindow::allButtons) {
	setUsingNativeTitleBar(true);

	previewHost.initialise();
	editorWindow.setPreviewPlayer(&previewHost.getPlayer());
	editorWindow.loadPrograms(SamplerDesktopPaths::getProgramFolder());

	setContentNonOwned(&editorWindow, true);
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
