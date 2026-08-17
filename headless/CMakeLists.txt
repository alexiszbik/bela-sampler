cmake_minimum_required(VERSION 3.22)

project(BelaSamplerHeadless VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SAMPLER_CORE_SOURCES
    ../platform/SamplerLog.cpp
    ../platform/SamplerBootstrap.cpp
    ../src/dsp/BeatRepeat.cpp
    ../src/dsp/BitCrusher.cpp
    ../src/dsp/BiquadFilter.cpp
    ../src/dsp/BufferMath.cpp
    ../src/dsp/CutoffHelper.cpp
    ../src/dsp/Flanger.cpp
    ../src/dsp/GainHelper.cpp
    ../src/dsp/Memory.cpp
    ../src/dsp/PitchHelper.cpp
    ../src/dsp/SmoothValue.cpp
    ../src/dsp/TriLfo.cpp
    ../src/engine/SamplePlayerPool.cpp
    ../src/engine/SamplerEngine.cpp
    ../src/engine/VoiceAllocator.cpp
    ../src/mix/FilterMixBus.cpp
    ../src/mix/FXMixBus.cpp
    ../src/mix/HatMixBus.cpp
    ../src/mix/MixBusArray.cpp
    ../src/mix/MixBusBase.cpp
    ../src/mix/MixBusFilterSection.cpp
    ../src/mix/MixBusNames.cpp
    ../src/mix/ReverbEffect.cpp
    ../src/mix/SnareMixBus.cpp
    ../src/mix/TomMixBus.cpp
    ../src/playback/SamplePlayer.cpp
    ../src/playback/SamplerVoice.cpp
    ../src/playback/sample/Sample.cpp
    ../src/program/Program.cpp
    ../src/program/ProgramBank.cpp
    ../src/program/ProgramJson.cpp
    ../src/program/ProgramMapJson.cpp
)

add_executable(sampler_headless
    ${SAMPLER_CORE_SOURCES}
    RtMidiBridge.cpp
    HeadlessSampler.cpp
    main.cpp
)

target_compile_definitions(sampler_headless PRIVATE SAMPLER_HEADLESS=1)

target_include_directories(sampler_headless PRIVATE
    ../platform
    ../src/program
    ../src/playback
    ../src/playback/sample
    ../src/engine
    ../src/mix
    ../src/dsp
    ../src/dsp/delay
    ../src/midi
    ../external/rtaudio
    ../external/rtmidi
    ../external/dr_wav
)

if(APPLE)
    find_library(COREAUDIO_LIBRARY CoreAudio REQUIRED)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(COREMIDI_LIBRARY CoreMIDI REQUIRED)
    target_link_libraries(sampler_headless PRIVATE
        ${COREAUDIO_LIBRARY}
        ${COREFOUNDATION_LIBRARY}
        ${COREMIDI_LIBRARY})
endif()

# RtAudio and RtMidi are compiled as single translation units.
set(RTAUDIO_CPP ../external/rtaudio/rtaudio.cpp)
set(RTMIDI_CPP ../external/rtmidi/rtmidi.cpp)

# Build RtAudio with CoreAudio backend on macOS.
set_source_files_properties(${RTAUDIO_CPP} PROPERTIES COMPILE_DEFINITIONS "__MACOSX_CORE__")
set_source_files_properties(${RTMIDI_CPP} PROPERTIES COMPILE_DEFINITIONS "__MACOSX_CORE__")

target_sources(sampler_headless PRIVATE ${RTAUDIO_CPP} ${RTMIDI_CPP})
