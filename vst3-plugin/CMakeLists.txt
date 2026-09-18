cmake_minimum_required(VERSION 3.20)
project(AutomasterSupreme VERSION 3.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
endif()

# Point to local JUCE installation
add_subdirectory("C:/Users/alfaswz/VeviDynamicEQ1/Build/_deps/juce-src" juce EXCLUDE_FROM_ALL)

juce_add_plugin(AutomasterSupreme
    COMPANY_NAME "vevi"
    BUNDLE_ID "com.vevi.automastersupreme31"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Vevi
    PLUGIN_CODE Au31
    FORMATS VST3 Standalone
    PRODUCT_NAME "AUTOMASTER SUPREME 3.1"
    DESCRIPTION "AI Mastering Suite v3.1 by vevi - Interactive Control Legend & Spotify Reference"
)

target_sources(AutomasterSupreme PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/MasteringDSP.h
)

target_compile_definitions(AutomasterSupreme PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(AutomasterSupreme PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
