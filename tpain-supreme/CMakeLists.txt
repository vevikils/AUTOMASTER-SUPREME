cmake_minimum_required(VERSION 3.22)

project(SupremeTunerRealTimeV3 VERSION 3.9.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE framework reference
add_subdirectory("C:/Users/alfaswz/VeviDynamicEQ1/Build/_deps/juce-src" juce EXCLUDE_FROM_ALL)

juce_add_plugin(SupremeTunerRealTimeV3
    COMPANY_NAME "vevi"
    BUNDLE_ID "com.vevi.supremetunerrealtimev39"
    PLUGIN_MANUFACTURER_CODE Vevi
    PLUGIN_CODE St39
    FORMATS VST3 Standalone
    PRODUCT_NAME "Supreme Tuner Real Time v3.9"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    IS_SYNTH FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    DESCRIPTION "Supreme Tuner Real Time v3.9 - By vevi - Ultra-Low Latency Vocal Auto-Tune + Hybrid Studio Console GUI"
)

target_sources(SupremeTunerRealTimeV3 PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/PitchEngine.h
    Source/VocalFXDSP.h
    Source/BgTextureData.h
)

target_compile_definitions(SupremeTunerRealTimeV3 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(SupremeTunerRealTimeV3 PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

if (MSVC)
    target_compile_options(SupremeTunerRealTimeV3 PRIVATE /utf-8 /bigobj)
endif()

add_executable(StressTest
    Tests/StressTest.cpp
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PitchEngine.h
    Source/VocalFXDSP.h
)
target_compile_definitions(StressTest PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)
target_link_libraries(StressTest PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
)
if (MSVC)
    target_compile_options(StressTest PRIVATE /utf-8 /bigobj)
endif()

