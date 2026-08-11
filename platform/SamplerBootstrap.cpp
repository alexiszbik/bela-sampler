#include "SamplerBootstrap.h"

#include "SamplerLog.h"

#include <dirent.h>
#include <sys/stat.h>

bool SamplerBootstrap::isWavFile(const std::string& filename) {
	if(filename.size() < 4) {
		return false;
	}

	const std::string ext = filename.substr(filename.size() - 4);
	return ext == ".wav" || ext == ".WAV";
}

bool SamplerBootstrap::loadSampleFile(const std::string& filePath,
	const std::string& programName,
	std::vector<Sample>& samples,
	size_t& totalRamBytes) {
	Sample sample;
	if(!sample.load(filePath, programName)) {
		SAMPLER_LOG("Failed to load %s\n", filePath.c_str());
		return false;
	}

	SAMPLER_LOG("Loaded %s (%u frames, %s, %u Hz, %.1f KB RAM)\n",
		programName.c_str(),
		sample.getLength(),
		sample.getChannelDescription().c_str(),
		sample.getSampleRate(),
		sample.getRamBytes() / 1024.f);

	totalRamBytes += sample.getRamBytes();
	samples.push_back(std::move(sample));
	return true;
}

bool SamplerBootstrap::loadSamplesInDirectory(const std::string& dirPath,
	const std::string& relativeDir,
	std::vector<Sample>& samples,
	size_t& totalRamBytes) {
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
			loadSamplesInDirectory(entryPath, programName, samples, totalRamBytes);
			continue;
		}

		if(!isWavFile(fileName)) {
			continue;
		}

		loadSampleFile(entryPath, programName, samples, totalRamBytes);
	}

	closedir(dir);
	return true;
}

bool SamplerBootstrap::init(std::vector<Sample>& samples,
	ProgramBank& programBank,
	SamplerEngine& engine,
	double sampleRate,
	size_t playerCount,
	const char* samplesFolder,
	const char* programFolder) {
	size_t totalRamBytes = 0;
	if(!loadSamplesInDirectory(samplesFolder, "", samples, totalRamBytes)) {
		SAMPLER_LOG("Could not open %s\n", samplesFolder);
		return false;
	}

	if(samples.empty()) {
		SAMPLER_LOG("No samples found in %s\n", samplesFolder);
		return false;
	}

	SAMPLER_LOG("Total samples RAM: %.1f KB (%.2f MB)\n",
		totalRamBytes / 1024.f,
		totalRamBytes / (1024.f * 1024.f));

	if(!programBank.load(programFolder, samples)) {
		return false;
	}

	engine.init(&programBank, sampleRate, playerCount);

	SAMPLER_LOG("Loaded %zu samples, %zu voice pool, %zu programs, active PC %d (%zu slots)\n",
		samples.size(),
		engine.getPlayerCount(),
		programBank.getLoadedProgramCount(),
		programBank.getActivePc(),
		programBank.getActiveProgram() != nullptr
			? programBank.getActiveProgram()->getSlotCount()
			: 0);

	return true;
}
