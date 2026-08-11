#include "MainWindow.h"
#include "SamplerAudioEngine.h"
#include "SamplerLog.h"

class BelaSamplerApplication : public juce::JUCEApplication
{
public:
	const juce::String getApplicationName() override { return "BelaSampler"; }
	const juce::String getApplicationVersion() override { return "1.0.0"; }
	bool moreThanOneInstanceAllowed() override { return false; }

	void initialise(const juce::String& commandLine) override {
		(void)commandLine;

		if(!audioEngine.initialise()) {
			SAMPLER_LOG("Failed to initialise sampler engine\n");
		}

		mainWindow = std::make_unique<MainWindow>(getApplicationName(), audioEngine);
	}

	void shutdown() override {
		mainWindow.reset();
		audioEngine.shutdown();
	}

	void systemRequestedQuit() override {
		quit();
	}

private:
	SamplerAudioEngine audioEngine;
	std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(BelaSamplerApplication)
