cmake_minimum_required(VERSION 3.22)
project(VEVI_PRO_Q VERSION 1.5.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find or fetch JUCE
find_package(JUCE CONFIG QUIET)
if(NOT JUCE_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 7.0.9
    )
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(VeviProQ
    COMPANY_NAME "VEVI Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Vevi
    PLUGIN_CODE VvPq
    FORMATS VST3 Standalone
    PRODUCT_NAME "VEVI Pro-Q"
)

juce_generate_juce_header(VeviProQ)

target_sources(VeviProQ PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/VeviDSP.h
)

target_compile_definitions(VeviProQ PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(VeviProQ PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

if (MSVC)
    target_compile_options(VeviProQ PRIVATE /utf-8)
endif()
