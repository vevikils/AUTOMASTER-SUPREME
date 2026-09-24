cmake_minimum_required(VERSION 3.22)

project(SupremeVocalBusCompressor VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE framework reference
add_subdirectory("C:/Users/alfaswz/VeviDynamicEQ1/Build/_deps/juce-src" juce EXCLUDE_FROM_ALL)

juce_add_plugin(SupremeVocalBusCompressor
    COMPANY_NAME "vevi"
    BUNDLE_ID "com.vevi.supremevocalbuscompressor"
    PLUGIN_MANUFACTURER_CODE Vevi
    PLUGIN_CODE Svbc
    FORMATS VST3 Standalone
    PRODUCT_NAME "Supreme Vocal Bus Compressor v1.0"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    IS_SYNTH FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    DESCRIPTION "Supreme Vocal Bus Compressor v1.0 - By vevi - CLA-2A Opto Style Vocal Glue & Tube Harmonic Sheen"
)

target_sources(SupremeVocalBusCompressor PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/OptoCompressorDSP.h
)

target_compile_definitions(SupremeVocalBusCompressor PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(SupremeVocalBusCompressor PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_dsp
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)
