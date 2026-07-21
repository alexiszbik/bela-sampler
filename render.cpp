/*
 * Sampler — Bela Gem C++ project
 * https://bela.io
 */

#include <Bela.h>
#include <dirent.h>

#include <string>
#include <vector>

#include "SamplePlayer.h"

static const char* kSamplesFolder = "samplesfolder";
static std::vector<SamplePlayer> gPlayers;

float mix[2] = {0.f, 0.f};

static bool isWavFile(const std::string& filename) {
	if(filename.size() < 4) {
		return false;
	}

	const std::string ext = filename.substr(filename.size() - 4);
	return ext == ".wav" || ext == ".WAV";
}

bool setup(BelaContext *context, void *userData) {
	DIR* dir = opendir(kSamplesFolder);
	if(!dir) {
		rt_printf("Could not open %s\n", kSamplesFolder);
		return false;
	}

	struct dirent* entry;
	while((entry = readdir(dir)) != nullptr)
	{
		const std::string fileName = entry->d_name;
		if(fileName == "." || fileName == "..") {
			continue;
		}
		if(!isWavFile(fileName)) {
			continue;
		}

		const std::string path = std::string(kSamplesFolder) + "/" + fileName;
		SamplePlayer player;
		if(!player.load(path)) {
			rt_printf("Failed to load %s\n", path.c_str());
			continue;
		}

		player.setPlayingSampleRate(context->audioSampleRate);

		rt_printf("Loaded %s (%u frames, %s, %u Hz)\n",
			fileName.c_str(),
			player.getLength(),
			player.getChannelDescription().c_str(),
			player.getSampleRate());

		player.setSpeed(0.7);
		gPlayers.push_back(std::move(player));
	}

	closedir(dir);

	if(gPlayers.empty()) {
		rt_printf("No samples found in %s\n", kSamplesFolder);
		return false;
	}

	rt_printf("Playing %zu samples in loop\n", gPlayers.size());
	return true;
}

void render(BelaContext *context, void *userData) {
	for(unsigned int n = 0; n < context->audioFrames; n++)
	{
		for(unsigned int channel = 0; channel < /*context->audioOutChannels*/2; channel++) {
			mix[channel] = 0;
		}

		for(SamplePlayer& player : gPlayers) {
			player.nextSamples(mix, 2);
		}

		for(unsigned int channel = 0; channel < /*context->audioOutChannels*/2; channel++) {
			audioWrite(context, n, channel, mix[channel]);
		}
	}
}

void cleanup(BelaContext *context, void *userData) {
	gPlayers.clear();
}
