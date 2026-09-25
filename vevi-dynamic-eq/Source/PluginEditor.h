#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <array>
#include "PluginProcessor.h"

// ==============================================================================
// Custom FabFilter Pro-Q Modern Dark LookAndFeel
// ==============================================================================
class ProQLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ProQLookAndFeel();
    ~ProQLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
};

// ==============================================================================
// Interactive Spectrum & Curve Plot Component with Piano Roll
// ==============================================================================
class ProQPlotComponent : public juce::Component, public juce::Timer
{
public:
    ProQPlotComponent(VeviProQAudioProcessor& p);
    ~ProQPlotComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    int getSelectedBand() const { return selectedBand; }
    void setSelectedBand(int idx);

    std::function<void(int)> onBandSelected;
    std::function<void(int)> onBandDragged;

    bool isPianoRollVisible = true;
    bool isRtaAnalyzerVisible = true;
    float currentDbScale = 12.0f; // ±12 dB by default

    static juce::Colour getBandColour(int bandIdx)
    {
        static const std::array<juce::Colour, 7> bandColours = {
            juce::Colour(0xffff3b5c), // Band 1: Vivid Coral Red
            juce::Colour(0xffff9500), // Band 2: Warm Amber Orange
            juce::Colour(0xffffcc00), // Band 3: Gold Yellow
            juce::Colour(0xff34d399), // Band 4: Electric Emerald Neon
            juce::Colour(0xff06b6d4), // Band 5: Cyan / Sky Blue
            juce::Colour(0xff3b82f6), // Band 6: Deep Indigo Sky
            juce::Colour(0xffa855f7)  // Band 7: Purple / Violet
        };
        return bandColours[(size_t)juce::jlimit(0, 6, bandIdx)];
    }

    static juce::String getNoteNameForFreq(float freq);

private:
    VeviProQAudioProcessor& processor;
    int selectedBand = 3; // Default to Band 4 (as in mockup)
    int draggedBand = -1;

    std::vector<float> smoothedPreScope;
    std::vector<float> smoothedPostScope;

    float getXForFrequency(float freq) const;
    float getFrequencyForX(float x) const;
    float getYForGain(float gainDB) const;
    float getGainForY(float y) const;
    juce::Point<float> getBandNodePos(int bandIdx) const;

    void drawPianoRoll(juce::Graphics& g, float yPos, float height);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProQPlotComponent)
};

// ==============================================================================
// Main Plugin Editor
// ==============================================================================
class VeviProQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    VeviProQAudioProcessorEditor (VeviProQAudioProcessor&);
    ~VeviProQAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    VeviProQAudioProcessor& audioProcessor;
    ProQLookAndFeel lnf;

    ProQPlotComponent plotComponent;

    // --- TOP HEADER CONTROLS ---
    // Professional clean top utility bar
    juce::TextButton btnAiMaster { "AI Master" };
    juce::TextButton btnPower { "POWER" };
    juce::TextButton btnFullscreen { "FULL" };

    // Preset selector bar
    juce::TextButton btnPrevPreset { "<" };
    juce::TextButton btnPresetName { "Default (Flat)" };
    juce::TextButton btnNextPreset { ">" };
    juce::TextButton btnABCompare { "A/B: A" };

    juce::TextButton btnTestAudio { "AUDITION" };
    juce::ComboBox sampleBeatCombo;

    void refreshPresetsUI();

    // Subheader Mode bar
    juce::TextButton btnPhaseZero { "Zero Latency" };
    juce::TextButton btnPhaseNatural { "Natural Phase" };
    juce::TextButton btnPhaseLinear { "Linear Phase" };

    juce::ToggleButton togglePianoRoll { "Piano Roll" };
    juce::ToggleButton toggleRtaAnalyzer { "RTA Analyzer" };

    juce::Slider speedSlider;
    juce::TextButton btnScale3dB { "+/-3dB" };
    juce::TextButton btnScale6dB { "+/-6dB" };
    juce::TextButton btnScale12dB { "+/-12dB" };
    juce::TextButton btnScale30dB { "+/-30dB" };

    // --- BOTTOM FLOATING INSPECTOR DOCK ---
    juce::Label inspectorBandBadge;
    juce::Label inspectorNoteBadge;
    juce::TextButton btnBandPower { "ON" };
    juce::TextButton btnBandSolo { "SOLO" };
    juce::TextButton btnBandPhaseInvert { "INV" };

    // Routing selector (ST, M, S, L, R)
    std::array<juce::TextButton, 5> routingButtons;

    // Shape Selectors (Bell, Low Shelf, High Shelf, Low Cut, High Cut, Notch, Band Pass, Tilt)
    std::array<juce::TextButton, 8> shapeButtons;

    // Slope Selectors (6dB, 12dB, 18dB, 24dB, 48dB)
    std::array<juce::TextButton, 5> slopeButtons;

    // Inspector Knobs (FREQ, GAIN, Q)
    juce::Slider freqSlider;
    juce::Label freqLabel { {}, "FREQ" };
    juce::Label freqValLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttach;

    juce::Slider gainSlider;
    juce::Label gainLabel { {}, "GAIN" };
    juce::Label gainValLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttach;

    juce::Slider qSlider;
    juce::Label qLabel { {}, "Q" };
    juce::Label qValLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qAttach;

    // Dynamic EQ Section
    juce::ToggleButton toggleDynamicEq { "DYNAMIC EQ" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dynEnableAttach;

    juce::Slider dynRangeSlider;
    juce::Label dynRangeLabel { {}, "RANGE" };
    juce::Label dynRangeValLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dynRangeAttach;

    juce::Slider dynThreshSlider;
    juce::Label dynThreshLabel { {}, "THRESH" };
    juce::Label dynThreshValLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dynThreshAttach;

    // Quick Band Selector Bottom Tabs (Band 1 to Band 7)
    std::array<juce::TextButton, 7> bandPills;

    // Master Output Controls (Right Strip)
    juce::Slider outputSlider;
    juce::Label outputLabel { {}, "OUTPUT" };
    juce::Label outputValLabel;
    juce::TextButton btnPhaseToggle { "PHASE" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttach;

    void updateInspector(int bandIdx);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VeviProQAudioProcessorEditor)
};
