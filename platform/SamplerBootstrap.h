#pragma once

#include "ProgramBank.h"
#include "SamplerEngine.h"
#include "Sample.h"

#include <cstddef>
#include <string>
#include <vector>

class SamplerBootstrap
{
public:
	static constexpr const char* kSamplesFolder = "samplesfolder";
	static constexpr const char* kProgramFolder = "program";
	static constexpr size_t kDefaultPlayerCount = 32;

	static bool init(std::vector<Sample>& samples,
		ProgramBank& programBank,
		SamplerEngine& engine,
		double sampleRate,
		size_t playerCount = kDefaultPlayerCount,
		const char* samplesFolder = kSamplesFolder,
		const char* programFolder = kProgramFolder);

private:
	static bool isWavFile(const std::string& filename);
	static bool loadSampleFile(const std::string& filePath,
		const std::string& programName,
		std::vector<Sample>& samples,
		size_t& totalRamBytes);
	static bool loadSamplesInDirectory(const std::string& dirPath,
		const std::string& relativeDir,
		std::vector<Sample>& samples,
		size_t& totalRamBytes);
};
