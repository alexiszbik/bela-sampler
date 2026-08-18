#include "HeadlessSampler.h"
#include "SamplerLog.h"

#include <atomic>
#include <csignal>
#include <iostream>
#include <string>

namespace {
constexpr const char* kDefaultVirtualPort = "Sampler Virtual";

void printUsage(const char* progName) {
	std::cerr << "Usage: " << progName << " [samplesFolder] [programFolder] [virtualPortName]\n"
			  << "  defaults: samplesfolder program \"" << kDefaultVirtualPort << "\"\n";
}
}

std::atomic<bool> gShouldStop {false};

void signalHandler(int signal) {
	(void)signal;
	gShouldStop = true;
}

int main(int argc, char* argv[]) {
	std::string samplesFolder = "samplesfolder";
	std::string programFolder = "program";
	std::string virtualPort = kDefaultVirtualPort;

	if(argc > 1) {
		samplesFolder = argv[1];
	}
	if(argc > 2) {
		programFolder = argv[2];
	}
	if(argc > 3) {
		virtualPort = argv[3];
	}

	if(argc > 4) {
		printUsage(argv[0]);
		return 1;
	}

	std::signal(SIGINT, signalHandler);
	std::signal(SIGTERM, signalHandler);

	HeadlessSampler sampler;
	if(!sampler.initialise(samplesFolder, programFolder, virtualPort)) {
		SAMPLER_LOG("Initialisation failed\n");
		return 1;
	}

	sampler.run();
	return 0;
}
