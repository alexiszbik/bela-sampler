#pragma once

void samplerLog(const char* format, ...) __attribute__((format(printf, 1, 2)));

#if defined(SAMPLER_HEADLESS)
using SamplerLogHandler = void (*)(const char* message);
void samplerLogSetHandler(SamplerLogHandler handler);
#endif

#define SAMPLER_LOG(...) samplerLog(__VA_ARGS__)
