cmake_minimum_required(VERSION 3.22)

project(BelaSampler VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../external/JUCE JUCE)

set(SAMPLER_CORE_SOURCES
    platform/LogBuffer.cpp
    ../platform/SamplerLog.cpp
    ../platform/SamplerBootstrap.cpp
    platform/ProgramWriter.cpp
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
    Source/SamplerDesktopPaths.cpp
    Source/JuceMidiBridge.cpp
    Source/SamplerMidiCollector.cpp
    Source/LogWindow.cpp
    Source/ProgramGridComponent.cpp
    Source/ProgramEditorWindow.cpp
    Source/Main.cpp
    Source/MainWindow.cpp
    Source/SamplerAudioEngine.cpp
    Source/SharedAudioFormatManager.cpp
    Source/SamplePreviewPlayer.cpp
)

juce_add_gui_app(BelaSampler
    PRODUCT_NAME "BelaSampler"
    COMPANY_NAME "YMNK"
    BUNDLE_ID "com.ymnk.belasampler")

target_sources(BelaSampler PRIVATE ${SAMPLER_CORE_SOURCES})

target_compile_definitions(BelaSampler PRIVATE SAMPLER_DESKTOP=1)

target_include_directories(BelaSampler PRIVATE
    platform
    ../platform
    ../src/program
    ../src/playback
    ../src/playback/sample
    ../src/engine
    ../src/mix
    ../src/dsp
    ../src/dsp/delay
    ../src/midi
    Source
)

target_link_libraries(BelaSampler
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_utils
        juce::juce_gui_basics
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(BelaSampler)
