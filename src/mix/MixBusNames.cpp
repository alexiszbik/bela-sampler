#include "MixBusNames.h"

#include <cctype>

namespace {
struct MixBusNicknameEntry {
	const char* nickname;
	MixBusIndex index;
};

constexpr MixBusNicknameEntry kMixBusNicknames[] = {
	{"master", kBusMaster},
	{"sample", kBusSample},
	{"kick", kBusKick},
	{"snare", kBusSnare},
	{"toms", kBusToms},
	{"hats", kBusHats},
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

bool mixBusIndexFromNickname(const std::string& name, MixBusIndex& outIndex) {
	for(const MixBusNicknameEntry& entry : kMixBusNicknames) {
		if(stringsEqualIgnoreCase(name, entry.nickname)) {
			outIndex = entry.index;
			return true;
		}
	}

	return false;
}

const char* mixBusNickname(MixBusIndex index) {
	for(const MixBusNicknameEntry& entry : kMixBusNicknames) {
		if(entry.index == index) {
			return entry.nickname;
		}
	}

	if(index >= kBusMaster && index < kBusCount) {
		return "?";
	}

	return nullptr;
}
