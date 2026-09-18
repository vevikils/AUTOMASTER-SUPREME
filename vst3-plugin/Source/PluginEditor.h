#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

// Custom Hardware-grade Tactile Rotary Knob LookAndFeel
class SupremeKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SupremeKnobLookAndFeel();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
};

class AutomasterSupremeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                              public juce::Timer
{
public:
    AutomasterSupremeAudioProcessorEditor(AutomasterSupremeAudioProcessor&);
    ~AutomasterSupremeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AutomasterSupremeAudioProcessor& audioProcessor;
    SupremeKnobLookAndFeel customKnobLAF;

    // GUI Top Bar
    juce::ComboBox presetBox;
    juce::TextButton autoMasterButton { "AI AUTO-MASTER" };

    // Module Bypass Buttons
    juce::ToggleButton bypassLowCut { "ON" };
    juce::ToggleButton bypassEQ { "ON" };
    juce::ToggleButton bypassSat { "ON" };
    juce::ToggleButton bypassMB { "ON" };
    juce::ToggleButton bypassLimiter { "ON" };

    // Knobs
    juce::Slider inGainSlider, lowCutSlider;
    juce::Slider eqSubSlider, eqLowMidSlider, eqMidSlider, eqClaritySlider, eqAirSlider;
    juce::Slider satDriveSlider, satWarmthSlider;
    juce::ComboBox satModeBox;
    juce::Slider mbLowSlider, mbMidSlider, mbHighSlider;
    juce::Slider stereoWidthSlider, monoMakerSlider;
    juce::Slider loudnessSlider, ceilingSlider, outGainSlider;

    // Labels
    juce::Label inGainLabel, lowCutLabel;
    juce::Label eqSubLabel, eqLowMidLabel, eqMidLabel, eqClarityLabel, eqAirLabel;
    juce::Label satDriveLabel, satWarmthLabel;
    juce::Label mbLowLabel, mbMidLabel, mbHighLabel;
    juce::Label stereoWidthLabel, monoMakerLabel;
    juce::Label loudnessLabel, ceilingLabel, outGainLabel;

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> inGainAttach, lowCutAttach;
    std::unique_ptr<ButtonAttachment> lowCutBypassAttach;

    std::unique_ptr<SliderAttachment> eqSubAttach, eqLowMidAttach, eqMidAttach, eqClarityAttach, eqAirAttach;
    std::unique_ptr<ButtonAttachment> eqBypassAttach;

    std::unique_ptr<SliderAttachment> satDriveAttach, satWarmthAttach;
    std::unique_ptr<ComboBoxAttachment> satModeAttach;
    std::unique_ptr<ButtonAttachment> satBypassAttach;

    std::unique_ptr<SliderAttachment> mbLowAttach, mbMidAttach, mbHighAttach;
    std::unique_ptr<ButtonAttachment> mbBypassAttach;

    std::unique_ptr<SliderAttachment> stereoWidthAttach, monoMakerAttach;
    std::unique_ptr<ButtonAttachment> stereoBypassAttach;

    std::unique_ptr<SliderAttachment> loudnessAttach, ceilingAttach, outGainAttach;
    std::unique_ptr<ButtonAttachment> limiterBypassAttach;

    // Telemetry and Drawing Caches
    std::array<float, 512> fftDisplayData {};
    std::array<float, 256> scopeL {}, scopeR {};
    float curPeakL = -100.0f;
    float curPeakR = -100.0f;
    float peakHoldL = -100.0f;
    float peakHoldR = -100.0f;
    float curLufs = -14.0f;
    float shortTermLufs = -14.0f;
    float curCrest = 10.0f;
    float curPhase = 1.0f;
    int agentAnimationTick = 0;

    void configureKnob(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix = "", juce::Colour accent = juce::Colour(0xff00f0ff));
    void configureBypassButton(juce::ToggleButton& btn);

    void drawSpectrumScreen(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawGoniometerScreen(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawMetersScreen(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawModuleCard(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);
    void drawAgentDeck(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawRackScrew(juce::Graphics& g, int x, int y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomasterSupremeAudioProcessorEditor)
};
