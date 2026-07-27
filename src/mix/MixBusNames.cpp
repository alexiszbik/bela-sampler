#include "MixBusNames.h"

#include "MixBusArray.h"

#include <cctype>

namespace {
struct MixBusNicknameEntry {
	const char* nickname;
	int index;
};

constexpr MixBusNicknameEntry kMixBusNicknames[] = {
	{"master", 0},
	{"sample", 1},
	{"kick", 2},
	{"snare", 3},
	{"toms", 4},
	{"hats", 5},
};

bool stringsEqualIgnoreCase(const std::string& a, const char* b) {
	if(b == nullptr) {
		return false;
	}

	size_t i = 0;
	for(; b[i] != '\0'; ++i) {
		if(i >= a.size()) {
			return false;
		}

		if(std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) {
			return false;
		}
	}

	return i == a.size();
}
}

bool mixBusIndexFromNickname(const std::string& name, int& outIndex) {
	for(const MixBusNicknameEntry& entry : kMixBusNicknames) {
		if(stringsEqualIgnoreCase(name, entry.nickname)) {
			outIndex = entry.index;
			return true;
		}
	}

	return false;
}

const char* mixBusNickname(int index) {
	for(const MixBusNicknameEntry& entry : kMixBusNicknames) {
		if(entry.index == index) {
			return entry.nickname;
		}
	}

	if(index >= 0 && index < static_cast<int>(MixBusArray::kBusCount)) {
		return "?";
	}

	return nullptr;
}
