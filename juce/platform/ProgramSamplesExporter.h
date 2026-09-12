#pragma once

#include "ProgramJson.h"

#include <string>
#include <vector>

class ProgramSamplesExporter
{
public:
	static std::string samplePathToIdentifier(const std::string& samplePath);
	static std::string programFileToNamespace(const std::string& programJsonFile);
	static std::string programFileToHeaderFilename(const std::string& programJsonFile);
	static bool writeHeader(const std::string& filepath,
		const std::string& programJsonFile,
		const std::vector<ProgramSlotDesc>& slots);
};
