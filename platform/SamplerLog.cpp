#include "SamplerLog.h"

#include <cstdarg>
#include <cstdio>

#if defined(SAMPLER_DESKTOP)
#include "LogBuffer.h"
#endif

#if defined(SAMPLER_BELA)
#include <Bela.h>
#endif

namespace {
constexpr size_t kLogLineLength = 256;
}

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
#elif defined(SAMPLER_DESKTOP)
	LogBuffer::getInstance().push(buffer);
#else
	std::fputs(buffer, stdout);
#endif
}
