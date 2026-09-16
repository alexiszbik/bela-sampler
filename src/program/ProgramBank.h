#pragma once

#include "Program.h"

#include <map>
#include <string>
#include <vector>

class ProgramBank
{
public:
	bool load(const std::string& programFolder,
		const std::vector<Sample>& samples,
		int preserveActivePc = -1);

	Program* getActiveProgram();
	const Program* getActiveProgram() const;
	int getActivePc() const { return activePc; }
	std::string getActiveProgramName() const;
	bool selectProgram(int pc);
	size_t getLoadedProgramCount() const { return programs.size(); }

private:
	std::vector<Program> programs;
	std::vector<std::string> programNames;
	std::map<int, size_t> pcToProgramIndex;
	size_t activeProgramIndex = 0;
	int activePc = 0;
};
