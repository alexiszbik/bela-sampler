#pragma once

#include <string>

bool mixBusIndexFromNickname(const std::string& name, int& outIndex);
const char* mixBusNickname(int index);
