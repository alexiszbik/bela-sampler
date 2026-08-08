#include "ProgramMapJson.h"

#include <Bela.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>

bool ProgramMapJson::readFile(const std::string& filepath) {
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

void ProgramMapJson::skipSpace() {
	while(*cursor != '\0' && std::isspace(static_cast<unsigned char>(*cursor))) {
		++cursor;
	}
}

bool ProgramMapJson::matchLiteral(char expected) {
	skipSpace();
	if(*cursor != expected) {
		return false;
	}

	++cursor;
	return true;
}

bool ProgramMapJson::parseInt(int& out) {
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

bool ProgramMapJson::parseQuotedString(std::string& out) {
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

bool ProgramMapJson::matchKey(const char* key) {
	const char* start = cursor;
	skipSpace();
	if(*cursor != '"') {
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
	skipSpace();
	if(*cursor != ':') {
		cursor = start;
		return false;
	}

	++cursor;
	return true;
}

void ProgramMapJson::skipValue() {
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

			if(*cursor == '\0') {
				return;
			}

			++cursor;
		} while(depth > 0);

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

			if(*cursor == '\0') {
				return;
			}

			++cursor;
		} while(depth > 0);

		return;
	}

	while(*cursor != '\0'
		&& *cursor != ','
		&& *cursor != '}'
		&& *cursor != ']') {
		++cursor;
	}
}

bool ProgramMapJson::parseProgramEntry(ProgramMapEntry& entry) {
	if(!matchLiteral('{')) {
		return false;
	}

	bool hasPc = false;
	bool hasFile = false;

	while(true) {
		skipSpace();
		if(*cursor == '}') {
			++cursor;
			break;
		}

		if(matchKey(kPc)) {
			if(!parseInt(entry.pc)) {
				return false;
			}

			hasPc = true;
		} else if(matchKey(kFile)) {
			if(!parseQuotedString(entry.file)) {
				return false;
			}

			hasFile = true;
		} else {
			skipValue();
		}

		skipSpace();
		if(*cursor == ',') {
			++cursor;
		}
	}

	return hasPc && hasFile;
}

bool ProgramMapJson::parseProgramsArray(std::vector<ProgramMapEntry>& entries) {
	if(!matchLiteral('[')) {
		return false;
	}

	while(true) {
		skipSpace();
		if(*cursor == ']') {
			++cursor;
			return true;
		}

		ProgramMapEntry entry;
		if(!parseProgramEntry(entry)) {
			return false;
		}

		entries.push_back(entry);

		skipSpace();
		if(*cursor == ',') {
			++cursor;
			continue;
		}

		if(*cursor == ']') {
			++cursor;
			return true;
		}

		return false;
	}
}

bool ProgramMapJson::parseRoot(ProgramMap& programMap) {
	if(!matchLiteral('{')) {
		return false;
	}

	bool hasPrograms = false;

	while(true) {
		skipSpace();
		if(*cursor == '}') {
			++cursor;
			break;
		}

		if(matchKey(kDefaultPc)) {
			if(!parseInt(programMap.defaultPc)) {
				return false;
			}
		} else if(matchKey(kPrograms)) {
			if(!parseProgramsArray(programMap.entries)) {
				return false;
			}

			hasPrograms = true;
		} else {
			skipValue();
		}

		skipSpace();
		if(*cursor == ',') {
			++cursor;
		}
	}

	return hasPrograms && !programMap.entries.empty();
}

bool ProgramMapJson::parseFile(const std::string& filepath, ProgramMap& programMap) {
	programMap = ProgramMap{};

	if(!readFile(filepath)) {
		rt_printf("ProgramMapJson: could not open %s\n", filepath.c_str());
		return false;
	}

	if(!parseRoot(programMap)) {
		rt_printf("ProgramMapJson: invalid map in %s\n", filepath.c_str());
		return false;
	}

	return true;
}
