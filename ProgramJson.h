#pragma once

#include "MuteGroup.h"

#include <string>
#include <vector>

enum class ProgramSlotMode {
	Poly,
	Mono,
	Gate
};

enum class ProgramSlotPlayMode {
	Normal,
	Granular
};

struct ProgramSlotDesc {
	int midiNote = 0;
	std::string sample;
	ProgramSlotMode mode = ProgramSlotMode::Poly;
	MuteGroup muteGroup = MuteGroup::None;
	float pitchSemitones = 0.f;
	ProgramSlotPlayMode playMode = ProgramSlotPlayMode::Normal;
	float granularSpeed = 1.f;
	bool reversed = false;
	float volumeDb = 0.f;
};

class ProgramJson
{
public:
	static constexpr const char* kModePoly = "poly";
	static constexpr const char* kModeMono = "mono";
	static constexpr const char* kModeGate = "gate";
	static constexpr const char* kPlayModeNormal = "normal";
	static constexpr const char* kPlayModeGranular = "granular";
	static constexpr const char* kMuteGroupA = "A";
	static constexpr const char* kMuteGroupB = "B";
	static constexpr const char* kMuteGroupC = "C";
	static constexpr const char* kMuteGroupD = "D";

	bool parseFile(const std::string& filepath, std::vector<ProgramSlotDesc>& slots);

private:
	bool readFile(const std::string& filepath);
	bool findSlotsArray();
	bool parseSlots(std::vector<ProgramSlotDesc>& slots);
	bool parseSlotObject(ProgramSlotDesc& slot);
	bool parseMode(ProgramSlotMode& mode);
	bool parseMuteGroup(MuteGroup& muteGroup);
	bool parsePitch(float& pitch);
	bool parsePlayMode(ProgramSlotPlayMode& playMode);
	bool parseReversed(bool& reversed);

	void skipSpace();
	bool matchLiteral(char expected);
	bool parseInt(int& out);
	bool parseFloat(float& out);
	bool parseQuotedString(std::string& out);
	bool matchKey(const char* key);
	void skipValue();

	std::string content;
	const char* cursor = nullptr;

	static constexpr const char* kMidiNote = "midiNote";
	static constexpr const char* kSample = "sample";
	static constexpr const char* kMode = "mode";
	static constexpr const char* kMuteGroup = "muteGroup";
	static constexpr const char* kPitch = "pitch";
	static constexpr const char* kPlayMode = "playmode";
	static constexpr const char* kGranularSpeed = "granularSpeed";
	static constexpr const char* kReversed = "reversed";
	static constexpr const char* kVolume = "volume";
	static constexpr const char* kSlots = "slots";
};
