#include "ProgramSamplesExporter.h"

#include <juce_core/juce_core.h>

#include <cctype>
#include <map>
#include <sstream>
#include <vector>

namespace {
std::string toLowerAscii(std::string text) {
	for(char& c : text) {
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	return text;
}

std::string capitalizeToken(const std::string& token) {
	if(token.empty()) {
		return {};
	}

	std::string result = toLowerAscii(token);
	result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
	return result;
}

std::vector<std::string> tokenizeStem(const std::string& stem) {
	std::vector<std::string> tokens;
	std::string current;

	for(char c : stem) {
		if(c == '_' || c == '-' || c == ' ') {
			if(!current.empty()) {
				tokens.push_back(current);
				current.clear();
			}
		} else {
			current += c;
		}
	}

	if(!current.empty()) {
		tokens.push_back(current);
	}

	return tokens;
}

std::string stemToCamelCase(const std::string& stem) {
	const std::vector<std::string> tokens = tokenizeStem(stem);
	if(tokens.empty()) {
		return {};
	}

	std::string result = toLowerAscii(tokens[0]);
	for(size_t i = 1; i < tokens.size(); ++i) {
		result += capitalizeToken(tokens[i]);
	}

	return result;
}

std::string sanitizeIdentifier(std::string identifier) {
	if(identifier.empty()) {
		return "sample";
	}

	if(std::isdigit(static_cast<unsigned char>(identifier[0])) != 0) {
		identifier = "n" + identifier;
	}

	std::string sanitized;
	sanitized.reserve(identifier.size());
	for(char c : identifier) {
		if(std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_') {
			sanitized += c;
		}
	}

	if(sanitized.empty()) {
		return "sample";
	}

	if(std::isdigit(static_cast<unsigned char>(sanitized[0])) != 0) {
		sanitized = "n" + sanitized;
	}

	return sanitized;
}

std::string makeUniqueIdentifier(const std::string& base, std::map<std::string, int>& usedCounts) {
	const int count = ++usedCounts[base];
	if(count == 1) {
		return base;
	}

	return base + std::to_string(count);
}

std::string fileNameStem(const std::string& path) {
	const size_t slashPos = path.find_last_of("/\\");
	const std::string fileName = slashPos == std::string::npos ? path : path.substr(slashPos + 1);
	const size_t dotPos = fileName.find_last_of('.');
	return dotPos == std::string::npos ? fileName : fileName.substr(0, dotPos);
}

std::string programStem(const std::string& programJsonFile) {
	return fileNameStem(programJsonFile);
}

std::string toPascalCase(const std::string& stem) {
	if(stem.empty()) {
		return "Program";
	}

	return capitalizeToken(stem);
}
}

std::string ProgramSamplesExporter::samplePathToIdentifier(const std::string& samplePath) {
	return sanitizeIdentifier(stemToCamelCase(fileNameStem(samplePath)));
}

std::string ProgramSamplesExporter::programFileToNamespace(const std::string& programJsonFile) {
	return toPascalCase(programStem(programJsonFile));
}

std::string ProgramSamplesExporter::programFileToHeaderFilename(const std::string& programJsonFile) {
	return programFileToNamespace(programJsonFile) + "Samples.h";
}

bool ProgramSamplesExporter::writeHeader(const std::string& filepath,
	const std::string& programJsonFile,
	const std::vector<ProgramSlotDesc>& slots) {
	std::ostringstream out;
	std::map<std::string, int> usedIdentifiers;
	int exportedCount = 0;

	const std::string namespaceName = programFileToNamespace(programJsonFile);

	out << "#pragma once\n\n";
	out << "#include <cstdint>\n\n";
	out << "namespace " << namespaceName << " {\n\n";

	for(const ProgramSlotDesc& slot : slots) {
		if(slot.sample.empty()) {
			continue;
		}

		const std::string baseIdentifier = samplePathToIdentifier(slot.sample);
		const std::string identifier = makeUniqueIdentifier(baseIdentifier, usedIdentifiers);

		out << "constexpr uint8_t " << identifier << " = " << slot.midiNote << ";\n\n";
		++exportedCount;
	}

	out << "}\n";

	if(exportedCount == 0) {
		return false;
	}

	const juce::File outputFile(filepath);
	outputFile.getParentDirectory().createDirectory();
	return outputFile.replaceWithText(out.str());
}
