#pragma once

#include <string>
#include <vector>

enum class ProgramSlotMode {
	Poly,
	Mono,
	Gate
};

struct ProgramSlotDesc {
	int midiNote = 0;
	std::string sample;
	ProgramSlotMode mode = ProgramSlotMode::Poly;
};

class ProgramJson
{
public:
	static constexpr const char* kModePoly = "poly";
	static constexpr const char* kModeMono = "mono";
	static constexpr const char* kModeGate = "gate";

	bool parseFile(const std::string& filepath, std::vector<ProgramSlotDesc>& slots);

private:
	bool readFile(const std::string& filepath);
	bool findSlotsArray();
	bool parseSlots(std::vector<ProgramSlotDesc>& slots);
	bool parseSlotObject(ProgramSlotDesc& slot);
	bool parseMode(ProgramSlotMode& mode);

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
	static constexpr const char* kMode = "mode";
	static constexpr const char* kSlots = "slots";
};
