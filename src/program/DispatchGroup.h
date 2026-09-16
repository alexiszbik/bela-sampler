#pragma once

#include <cstddef>

enum class DispatchGroup {
	None,
	A,
	B,
	C,
	D
};

inline size_t dispatchGroupIndex(DispatchGroup group) {
	return static_cast<size_t>(group) - 1u;
}

inline bool isDispatchGroupAssigned(DispatchGroup group) {
	return group != DispatchGroup::None;
}
