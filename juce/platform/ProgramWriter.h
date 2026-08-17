#pragma once

#include "ProgramJson.h"
#include "ProgramMapJson.h"

#include <string>
#include <vector>

class ProgramWriter
{
public:
	static bool writeProgram(const std::string& filepath, const std::vector<ProgramSlotDesc>& slots);
	static bool writeProgramMap(const std::string& filepath, const ProgramMap& programMap);
};
