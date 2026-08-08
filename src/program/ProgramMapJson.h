#pragma once

#include <string>
#include <vector>

struct ProgramMapEntry {
	int pc = 0;
	std::string file;
};

struct ProgramMap {
	int defaultPc = 0;
	std::vector<ProgramMapEntry> entries;
};

class ProgramMapJson
{
public:
	bool parseFile(const std::string& filepath, ProgramMap& programMap);

private:
	bool readFile(const std::string& filepath);
	void skipSpace();
	bool matchLiteral(char expected);
	bool parseInt(int& out);
	bool parseQuotedString(std::string& out);
	bool matchKey(const char* key);
	void skipValue();
	bool parseProgramEntry(ProgramMapEntry& entry);
	bool parseProgramsArray(std::vector<ProgramMapEntry>& entries);
	bool parseRoot(ProgramMap& programMap);

	std::string content;
	const char* cursor = nullptr;

	static constexpr const char* kDefaultPc = "defaultPc";
	static constexpr const char* kPrograms = "programs";
	static constexpr const char* kPc = "pc";
	static constexpr const char* kFile = "file";
};
