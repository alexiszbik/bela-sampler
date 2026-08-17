cmake_minimum_required(VERSION 3.22)

project(BelaSampler VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../external/JUCE JUCE)

set(SAMPLER_EDITOR_SOURCES
    ../platform/SamplerLog.cpp
    platform/ProgramWriter.cpp
    ../src/program/ProgramJson.cpp
    ../src/program/ProgramMapJson.cpp
    ../src/dsp/PitchHelper.cpp
    ../src/mix/MixBusNames.cpp
    Source/SamplerDesktopPaths.cpp
    Source/SharedAudioFormatManager.cpp
    Source/SamplePreviewPlayer.cpp
    Source/PreviewAudioHost.cpp
    Source/ProgramGridComponent.cpp
    Source/ProgramEditorWindow.cpp
    Source/MainWindow.cpp
    Source/Main.cpp
)

juce_add_gui_app(BelaSampler
    PRODUCT_NAME "BelaSamplerEditor"
    COMPANY_NAME "YMNK"
    BUNDLE_ID "com.ymnk.belasampler.editor")

target_sources(BelaSampler PRIVATE ${SAMPLER_EDITOR_SOURCES})

target_compile_definitions(BelaSampler PRIVATE SAMPLER_DESKTOP=1)

target_include_directories(BelaSampler PRIVATE
    platform
    ../platform
    ../src/program
    ../src/mix
    ../src/dsp
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
