#include "SharedAudioFormatManager.h"

SharedAudioFormatManager::SharedAudioFormatManager() {
	manager.registerBasicFormats();
}

juce::AudioFormatManager& SharedAudioFormatManager::get() {
	static SharedAudioFormatManager instance;
	return instance.manager;
}
