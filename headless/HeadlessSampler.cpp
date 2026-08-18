#include "HeadlessSampler.h"

#include "SamplerBootstrap.h"
#include "SamplerLog.h"

#include <chrono>
#include <thread>

extern std::atomic<bool> gShouldStop;

namespace {
constexpr double kSampleRate = 44100.0;
constexpr unsigned int kBlockSize = 512;
constexpr size_t kNumPlayers = 32;
constexpr unsigned int kOutputChannels = 2;

const char* errorTypeName(RtAudioErrorType type) {
	switch(type) {
		case RTAUDIO_NO_ERROR: return "no error";
		case RTAUDIO_WARNING: return "warning";
		case RTAUDIO_UNKNOWN_ERROR: return "unknown error";
		case RTAUDIO_NO_DEVICES_FOUND: return "no devices found";
		case RTAUDIO_INVALID_DEVICE: return "invalid device";
		case RTAUDIO_DEVICE_DISCONNECT: return "device disconnect";
		case RTAUDIO_MEMORY_ERROR: return "memory error";
		case RTAUDIO_INVALID_PARAMETER: return "invalid parameter";
		case RTAUDIO_INVALID_USE: return "invalid use";
		case RTAUDIO_DRIVER_ERROR: return "driver error";
		case RTAUDIO_SYSTEM_ERROR: return "system error";
		case RTAUDIO_THREAD_ERROR: return "thread error";
		default: return "unspecified error";
	}
}
}

HeadlessSampler::HeadlessSampler()
	: midiBridge(engine) {
}

HeadlessSampler::~HeadlessSampler() {
	stop();
}

bool HeadlessSampler::initialise(const std::string& samplesFolder,
	const std::string& programFolder,
	const std::string& virtualPortName) {
	if(!SamplerBootstrap::init(samples,
			programBank,
			engine,
			kSampleRate,
			kNumPlayers,
			samplesFolder.c_str(),
			programFolder.c_str())) {
		SAMPLER_LOG("Bootstrap failed\n");
		return false;
	}

	dac = std::make_unique<RtAudio>();
	if(dac->getDeviceCount() == 0) {
		SAMPLER_LOG("No audio devices available\n");
		return false;
	}

	RtAudio::StreamParameters outParams;
	outParams.deviceId = dac->getDefaultOutputDevice();
	outParams.nChannels = kOutputChannels;
	outParams.firstChannel = 0;

	RtAudio::StreamOptions options;
	options.flags = RTAUDIO_SCHEDULE_REALTIME | RTAUDIO_MINIMIZE_LATENCY;
	options.priority = 40;
	options.streamName = "BelaSamplerHeadless";

	unsigned int blockSize = kBlockSize;
	const RtAudioErrorType openError = dac->openStream(&outParams, nullptr, RTAUDIO_FLOAT32,
		kSampleRate, &blockSize, &HeadlessSampler::audioCallback, this, &options);
	if(openError != RTAUDIO_NO_ERROR) {
		SAMPLER_LOG("RtAudio openStream error: %s\n", errorTypeName(openError));
		return false;
	}

	midiIn = std::make_unique<RtMidiIn>();
	midiIn->setCallback(&HeadlessSampler::midiCallback, this);
	try {
		midiIn->openVirtualPort(virtualPortName);
		SAMPLER_LOG("Virtual MIDI input port created: %s\n", virtualPortName.c_str());
		SAMPLER_LOG("Connect your MIDI source to this port.\n");
	} catch(RtMidiError& e) {
		SAMPLER_LOG("Could not create virtual MIDI port: %s\n", e.getMessage().c_str());
		return false;
	}

	return true;
}

void HeadlessSampler::run() {
	if(running) {
		return;
	}

	const RtAudioErrorType startError = dac->startStream();
	if(startError != RTAUDIO_NO_ERROR) {
		SAMPLER_LOG("RtAudio startStream error: %s\n", errorTypeName(startError));
		return;
	}

	running = true;
	SAMPLER_LOG("Headless sampler running (sampleRate=%.0f blockSize=%u)\n",
		kSampleRate, kBlockSize);
	SAMPLER_LOG("Press Ctrl-C to quit\n");

	while(running && !gShouldStop) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	stop();
}

void HeadlessSampler::stop() {
	if(!running) {
		return;
	}

	running = false;

	if(midiIn != nullptr) {
		try {
			midiIn->cancelCallback();
			midiIn->closePort();
		} catch(RtMidiError& e) {
			SAMPLER_LOG("MIDI closeVirtualPort error: %s\n", e.getMessage().c_str());
		}
	}

	if(dac != nullptr) {
		if(dac->isStreamRunning()) {
			dac->stopStream();
		}
		if(dac->isStreamOpen()) {
			dac->closeStream();
		}
	}
}

void HeadlessSampler::midiCallback(double timestamp,
	std::vector<unsigned char>* message, void* userData) {
	auto* self = static_cast<HeadlessSampler*>(userData);
	if(self != nullptr && message != nullptr) {
		self->midiBridge.onMessage(timestamp, *message);
	}
}

int HeadlessSampler::audioCallback(void* outputBuffer, void* inputBuffer,
	unsigned int nFrames, double streamTime,
	RtAudioStreamStatus status, void* userData) {
	(void)inputBuffer;
	(void)streamTime;
	(void)status;

	auto* self = static_cast<HeadlessSampler*>(userData);
	if(self == nullptr) {
		return 0;
	}

	self->midiBridge.drain();

	auto* out = static_cast<float*>(outputBuffer);
	float mix[MixBusArray::kMasterChannelCount] = {};

	for(unsigned int n = 0; n < nFrames; ++n) {
		for(size_t channel = 0; channel < MixBusArray::kMasterChannelCount; ++channel) {
			mix[channel] = 0.f;
		}

		self->engine.nextSamples(mix, MixBusArray::kMasterChannelCount);

		const float monoMix = mix[4] + mix[5] + mix[6] + mix[7];
		out[n * kOutputChannels + 0] = mix[0] + mix[2] + monoMix;
		out[n * kOutputChannels + 1] = mix[1] + mix[3] + monoMix;
	}

	return 0;
}
