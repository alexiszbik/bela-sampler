#include "MainWindow.h"
#include "SamplerDesktopPaths.h"
#include "SamplerLog.h"

MainWindow::MainWindow(juce::String name, SamplerAudioEngine& audioEngine)
	: DocumentWindow(name,
		juce::Desktop::getInstance().getDefaultLookAndFeel()
			.findColour(juce::ResizableWindow::backgroundColourId),
		DocumentWindow::allButtons),
	  engine(audioEngine),
	  tabs(juce::TabbedButtonBar::TabsAtTop) {
	setUsingNativeTitleBar(true);

	editorWindow.setPreviewPlayer(&engine.getPreviewPlayer());
	editorWindow.loadPrograms(SamplerDesktopPaths::getProgramFolder());
	editorWindow.onReload = [this] { return engine.reload(); };

	tabs.addTab("Log", juce::Colour(0xff1e1e1e), &logWindow, false);
	tabs.addTab("Program Editor", juce::Colour(0xff1e1e1e), &editorWindow, false);

	setContentNonOwned(&tabs, true);
	setResizable(true, true);
	centreWithSize(1000, 700);
	setVisible(true);

	setName("Bela Sampler Desktop - "
		+ juce::String(static_cast<int>(engine.getSamples().size())) + " samples, "
		+ juce::String(static_cast<int>(engine.getProgramBank().getLoadedProgramCount())) + " programs");
}

void MainWindow::closeButtonPressed() {
	juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
