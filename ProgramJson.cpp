#include "ProgramJson.h"

#include "PitchHelper.h"

#include <Bela.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>

bool ProgramJson::readFile(const std::string& filepath) {
	FILE* file = fopen(filepath.c_str(), "rb");
	if(file == nullptr) {
		return false;
	}

	if(fseek(file, 0, SEEK_END) != 0) {
		fclose(file);
		return false;
	}

	const long fileSize = ftell(file);
	if(fileSize < 0) {
		fclose(file);
		return false;
	}

	if(fseek(file, 0, SEEK_SET) != 0) {
		fclose(file);
		return false;
	}

	content.resize(static_cast<size_t>(fileSize));
	const size_t bytesRead = fread(&content[0], 1, static_cast<size_t>(fileSize), file);
	fclose(file);

	if(bytesRead != static_cast<size_t>(fileSize)) {
		content.clear();
		return false;
	}

	cursor = content.c_str();
	return true;
}

void ProgramJson::skipSpace() {
	while(*cursor != '\0' && std::isspace(static_cast<unsigned char>(*cursor))) {
		++cursor;
	}
}

bool ProgramJson::matchLiteral(char expected) {
	skipSpace();
	if(*cursor != expected) {
		return false;
	}

	++cursor;
	return true;
}

bool ProgramJson::parseInt(int& out) {
	skipSpace();
	char* end = nullptr;
	const long value = std::strtol(cursor, &end, 10);
	if(end == cursor) {
		return false;
	}

	out = static_cast<int>(value);
	cursor = end;
	return true;
}

bool ProgramJson::parseFloat(float& out) {
	skipSpace();
	char* end = nullptr;
	const double value = std::strtod(cursor, &end);
	if(end == cursor) {
		return false;
	}

	out = static_cast<float>(value);
	cursor = end;
	return true;
}

bool ProgramJson::parseQuotedString(std::string& out) {
	skipSpace();
	if(*cursor != '"') {
		return false;
	}

	++cursor;
	out.clear();
	while(*cursor != '\0' && *cursor != '"') {
		if(*cursor == '\\') {
			++cursor;
			if(*cursor == '\0') {
				return false;
			}
		}

		out += *cursor;
		++cursor;
	}

	if(*cursor != '"') {
		return false;
	}

	++cursor;
	return true;
}

bool ProgramJson::matchKey(const char* key) {
	const char* start = cursor;
	skipSpace();
	if(*cursor != '"') {
		cursor = start;
		return false;
	}

	++cursor;
	while(*key != '\0') {
		if(*cursor != *key) {
			cursor = start;
			return false;
		}

		++cursor;
		++key;
	}

	if(*cursor != '"') {
		cursor = start;
		return false;
	}

	++cursor;
	return true;
}

void ProgramJson::skipValue() {
	skipSpace();
	if(*cursor == '"') {
		std::string ignored;
		parseQuotedString(ignored);
		return;
	}

	if(*cursor == '{') {
		int depth = 0;
		do {
			if(*cursor == '{') {
				++depth;
			} else if(*cursor == '}') {
				--depth;
			}

			++cursor;
		} while(*cursor != '\0' && depth > 0);
		return;
	}

	if(*cursor == '[') {
		int depth = 0;
		do {
			if(*cursor == '[') {
				++depth;
			} else if(*cursor == ']') {
				--depth;
			}

			++cursor;
		} while(*cursor != '\0' && depth > 0);
		return;
	}

	while(*cursor != '\0' && *cursor != ',' && *cursor != '}' && *cursor != ']') {
		++cursor;
	}
}

bool ProgramJson::parseMode(ProgramSlotMode& mode) {
	std::string modeName;
	if(!parseQuotedString(modeName)) {
		return false;
	}

	if(modeName == kModeMono) {
		mode = ProgramSlotMode::Mono;
	} else if(modeName == kModePoly) {
		mode = ProgramSlotMode::Poly;
	} else if(modeName == kModeGate) {
		mode = ProgramSlotMode::Gate;
	} else {
		return false;
	}

	return true;
}

bool ProgramJson::parseMuteGroup(MuteGroup& muteGroup) {
	std::string groupName;
	if(!parseQuotedString(groupName)) {
		return false;
	}

	if(groupName == kMuteGroupA) {
		muteGroup = MuteGroup::A;
	} else if(groupName == kMuteGroupB) {
		muteGroup = MuteGroup::B;
	} else if(groupName == kMuteGroupC) {
		muteGroup = MuteGroup::C;
	} else if(groupName == kMuteGroupD) {
		muteGroup = MuteGroup::D;
	} else {
		return false;
	}

	return true;
}

bool ProgramJson::parsePitch(float& pitch) {
	if(!parseFloat(pitch)) {
		return false;
	}

	pitch = clampPitchSemitones(pitch);
	return true;
}

bool ProgramJson::parseSlotObject(ProgramSlotDesc& slot) {
	if(!matchLiteral('{')) {
		return false;
	}

	bool hasNote = false;
	bool hasSample = false;
	bool hasMuteGroup = false;

	while(*cursor != '\0') {
		skipSpace();
		if(*cursor == '}') {
			++cursor;
			break;
		}

		if(matchKey(kMidiNote)) {
			if(!matchLiteral(':') || !parseInt(slot.midiNote)) {
				return false;
			}
			hasNote = true;
		} else if(matchKey(kSample)) {
			if(!matchLiteral(':') || !parseQuotedString(slot.sample)) {
				return false;
			}
			hasSample = true;
		} else if(matchKey(kMode)) {
			if(!matchLiteral(':') || !parseMode(slot.mode)) {
				return false;
			}
		} else if(matchKey(kMuteGroup)) {
			if(!matchLiteral(':') || !parseMuteGroup(slot.muteGroup)) {
				return false;
			}
			hasMuteGroup = slot.muteGroup != MuteGroup::None;
		} else if(matchKey(kPitch)) {
			if(!matchLiteral(':') || !parsePitch(slot.pitchSemitones)) {
				return false;
			}
		} else {
			skipValue();
		}

		skipSpace();
		if(*cursor == ',') {
			++cursor;
		}
	}

	return hasNote && (hasSample || hasMuteGroup);
}

bool ProgramJson::findSlotsArray() {
	cursor = content.c_str();
	skipSpace();
	if(!matchLiteral('{')) {
		return false;
	}

	while(*cursor != '\0') {
		skipSpace();
		if(*cursor == '}') {
			return false;
		}

		if(matchKey(kSlots)) {
			if(!matchLiteral(':') || !matchLiteral('[')) {
				return false;
			}

			return true;
		}

		skipValue();
		skipSpace();
		if(*cursor == ',') {
			++cursor;
		}
	}

	return false;
}

bool ProgramJson::parseSlots(std::vector<ProgramSlotDesc>& slots) {
	while(*cursor != '\0') {
		skipSpace();
		if(*cursor == ']') {
			++cursor;
			break;
		}

		ProgramSlotDesc slot;
		if(!parseSlotObject(slot)) {
			return false;
		}

		slots.push_back(slot);

		skipSpace();
		if(*cursor == ',') {
			++cursor;
		}
	}

	return !slots.empty();
}

bool ProgramJson::parseFile(const std::string& filepath, std::vector<ProgramSlotDesc>& slots) {
	slots.clear();
	content.clear();
	cursor = nullptr;

	if(!readFile(filepath)) {
		rt_printf("ProgramJson: could not open %s\n", filepath.c_str());
		return false;
	}

	if(!findSlotsArray()) {
		rt_printf("ProgramJson: missing slots array in %s\n", filepath.c_str());
		return false;
	}

	if(!parseSlots(slots)) {
		rt_printf("ProgramJson: invalid slot in %s\n", filepath.c_str());
		return false;
	}

	return true;
}
