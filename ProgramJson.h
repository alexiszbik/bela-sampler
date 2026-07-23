#pragma once

#include <string>
#include <vector>

struct ProgramSlotDesc {
	int midiNote = 0;
	std::string sample;
};

class ProgramJson
{
public:
	bool parseFile(const std::string& filepath, std::vector<ProgramSlotDesc>& slots);

private:
	bool readFile(const std::string& filepath);
	bool findSlotsArray();
	bool parseSlots(std::vector<ProgramSlotDesc>& slots);
	bool parseSlotObject(ProgramSlotDesc& slot);

	void skipSpace();
	bool matchLiteral(char expected);
	bool parseInt(int& out);
	bool parseQuotedString(std::string& out);
	bool matchKey(const char* key);
	void skipValue();

	std::string content;
	const char* cursor = nullptr;

	static constexpr const char* kMidiNote = "midiNote";
	static constexpr const char* kSample = "sample";
	static constexpr const char* kSlots = "slots";
};
