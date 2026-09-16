#include "SamplerLog.h"

#include <cstdarg>
#include <cstdio>

#if defined(SAMPLER_BELA)
#include <Bela.h>
#endif

namespace {
constexpr size_t kLogLineLength = 256;

#if defined(SAMPLER_HEADLESS)
SamplerLogHandler gLogHandler = nullptr;
#endif
}

#if defined(SAMPLER_HEADLESS)
void samplerLogSetHandler(SamplerLogHandler handler) {
	gLogHandler = handler;
}
#endif

void samplerLog(const char* format, ...) {
	char buffer[kLogLineLength];

	va_list args;
	va_start(args, format);
	const int written = std::vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if(written <= 0) {
		return;
	}

#if defined(SAMPLER_BELA)
	rt_printf("%s", buffer);
#elif defined(SAMPLER_HEADLESS)
	if(gLogHandler != nullptr) {
		gLogHandler(buffer);
	} else {
		std::fputs(buffer, stdout);
		std::fflush(stdout);
	}
#else
	std::fputs(buffer, stdout);
	std::fflush(stdout);
#endif
}
