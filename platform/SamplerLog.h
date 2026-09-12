#pragma once

void samplerLog(const char* format, ...) __attribute__((format(printf, 1, 2)));

#define SAMPLER_LOG(...) samplerLog(__VA_ARGS__)
