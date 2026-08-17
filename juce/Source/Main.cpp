#include "MainWindow.h"
#include "SamplerLog.h"

class BelaSamplerEditorApplication : public juce::JUCEApplication
{
public:
	const juce::String getApplicationName() override { return "BelaSamplerEditor"; }
	const juce::String getApplicationVersion() override { return "1.0.0"; }
	bool moreThanOneInstanceAllowed() override { return false; }

	void initialise(const juce::String& commandLine) override {
		(void)commandLine;
		mainWindow = std::make_unique<MainWindow>("Bela Sampler Editor");
	}

	void shutdown() override {
		mainWindow.reset();
	}

	void systemRequestedQuit() override {
		quit();
	}

private:
	std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(BelaSamplerEditorApplication)
