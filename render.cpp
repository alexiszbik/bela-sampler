/*
 * Sampler — Bela Gem C++ project
 * https://bela.io
 */

#include <Bela.h>
#include <dirent.h>

#include <string>
#include <vector>

#include <sys/stat.h>

#include "Sample.h"
#include "ProgramBank.h"
#include "SamplerEngine.h"
#include "MidiInput.h"
#include "MixBusArray.h"

static const char* kSamplesFolder = "samplesfolder";
static const char* kProgramFolder = "program";
static const size_t kNumPlayers = 32;

static std::vector<Sample> gSamples;
static ProgramBank gProgramBank;
static SamplerEngine gEngine;
static MidiInput gMidiInput;

float mix[MixBusArray::kMasterChannelCount] = {};

const int kOutChannelStart = 2;

static bool isWavFile(const std::string& filename) {
	if(filename.size() < 4) {
		return false;
	}

	const std::string ext = filename.substr(filename.size() - 4);
	return ext == ".wav" || ext == ".WAV";
}

static bool loadSampleFile(const std::string& filePath, const std::string& programName, size_t& totalRamBytes) {
	Sample sample;
	if(!sample.load(filePath, programName)) {
		rt_printf("Failed to load %s\n", filePath.c_str());
		return false;
	}

	rt_printf("Loaded %s (%u frames, %s, %u Hz, %.1f KB RAM)\n",
		programName.c_str(),
		sample.getLength(),
		sample.getChannelDescription().c_str(),
		sample.getSampleRate(),
		sample.getRamBytes() / 1024.f);

	totalRamBytes += sample.getRamBytes();
	gSamples.push_back(std::move(sample));
	return true;
}

static bool loadSamplesInDirectory(const std::string& dirPath, const std::string& relativeDir, size_t& totalRamBytes) {
	DIR* dir = opendir(dirPath.c_str());
	if(dir == nullptr) {
		return false;
	}

	struct dirent* entry;
	while((entry = readdir(dir)) != nullptr) {
		const std::string fileName = entry->d_name;
		if(fileName == "." || fileName == "..") {
			continue;
		}

		const std::string entryPath = dirPath + "/" + fileName;
		const std::string programName = relativeDir.empty() ? fileName : relativeDir + "/" + fileName;

		struct stat entryStat;
		if(stat(entryPath.c_str(), &entryStat) != 0) {
			continue;
		}

		if(S_ISDIR(entryStat.st_mode)) {
			loadSamplesInDirectory(entryPath, programName, totalRamBytes);
			continue;
		}

		if(!isWavFile(fileName)) {
			continue;
		}

		loadSampleFile(entryPath, programName, totalRamBytes);
	}

	closedir(dir);
	return true;
}

bool setup(BelaContext *context, void *userData) {
	size_t totalRamBytes = 0;
	if(!loadSamplesInDirectory(kSamplesFolder, "", totalRamBytes)) {
		rt_printf("Could not open %s\n", kSamplesFolder);
		return false;
	}

	if(gSamples.empty()) {
		rt_printf("No samples found in %s\n", kSamplesFolder);
		return false;
	}

	rt_printf("Total samples RAM: %.1f KB (%.2f MB)\n",
		totalRamBytes / 1024.f,
		totalRamBytes / (1024.f * 1024.f));

	if(!gProgramBank.load(kProgramFolder, gSamples)) {
		return false;
	}

	gEngine.init(&gProgramBank, context->audioSampleRate, kNumPlayers);

	rt_printf("Loaded %zu samples, %zu voice pool, %zu programs, active PC %d (%zu slots)\n",
		gSamples.size(),
		gEngine.getPlayerCount(),
		gProgramBank.getLoadedProgramCount(),
		gProgramBank.getActivePc(),
		gProgramBank.getActiveProgram() != nullptr
			? gProgramBank.getActiveProgram()->getSlotCount()
			: 0);

	gMidiInput.setup(&gEngine);
	return true;
}

void render(BelaContext *context, void *userData) {
	for(unsigned int n = 0; n < context->audioFrames; n++)
	{
		for(unsigned int channel = 0; channel < MixBusArray::kMasterChannelCount; channel++) {
			mix[channel] = 0;
		}

		gEngine.nextSamples(mix, MixBusArray::kMasterChannelCount);

		for(unsigned int channel = 0; channel < MixBusArray::kMasterChannelCount; channel++) {
			audioWrite(context, n, channel + kOutChannelStart, mix[channel]);
		}
	}
}

void cleanup(BelaContext *context, void *userData) {
	gSamples.clear();
}
