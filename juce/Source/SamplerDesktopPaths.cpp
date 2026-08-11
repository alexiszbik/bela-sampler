#include "SamplerDesktopPaths.h"

#include <juce_core/juce_core.h>

namespace {
const juce::File& getRepoRoot() {
	static const juce::File repoRoot = juce::File(__FILE__)
		.getParentDirectory()
		.getParentDirectory()
		.getParentDirectory();

	return repoRoot;
}
}

std::string SamplerDesktopPaths::getSamplesFolder() {
	return getRepoRoot().getChildFile("samplesfolder").getFullPathName().toStdString();
}

std::string SamplerDesktopPaths::getProgramFolder() {
	return getRepoRoot().getChildFile("program").getFullPathName().toStdString();
}
