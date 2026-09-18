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

// Interactive Legend / Inspector Data Model
struct ControlLegendInfo
{
    juce::String name;
    juce::String moduleName;
    juce::String rangeStr;
    juce::String defaultStr;
    juce::String description;
    juce::String spotifyTip;
    juce::Colour accent;
};

class AutomasterSupremeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                              public juce::Timer
{
public:
    AutomasterSupremeAudioProcessorEditor(AutomasterSupremeAudioProcessor&);
    ~AutomasterSupremeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void paintOverChildren(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;

private:
    AutomasterSupremeAudioProcessor& audioProcessor;
    SupremeKnobLookAndFeel customKnobLAF;
    juce::TooltipWindow tooltipWindow { this, 450 };

    // Interactive Legend / Inspector State
    std::vector<std::pair<juce::Component*, ControlLegendInfo>> controlLegends;
    ControlLegendInfo activeLegend;
    juce::String activeControlValue;
    bool isControlActive = false;
    bool showLegendDrawer = false;

    void registerLegend(juce::Component& comp, const ControlLegendInfo& info);
    void updateHoverFromComponent(juce::Component* comp);

    // GUI Top Bar
    juce::ComboBox presetBox;
    juce::TextButton autoMasterButton { "AI AUTO-MASTER" };
    juce::TextButton legendButton { "📖 LEYENDA DE CONTROLES" };
    juce::TextButton closeLegendButton { "CERRAR GUÍA ✕" };

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
    std::array<float, AudioConstants::FFT_SIZE / 2> fftDisplayData {};
    std::array<float, 256> scopeL {}, scopeR {};
    float curPhase = 1.0f;
    int agentAnimationTick = 0;

    // 0.5-second meter refresh tracking (updates every 500ms for stable reading)
    int meterTimerTickCount = 0;
    float accumPeakL = -100.0f;
    float accumPeakR = -100.0f;
    float accumMomentaryLufs = 0.0f;
    float accumCrest = 0.0f;
    int accumSampleCount = 0;

    // Stable values displayed on the right-hand meters (refreshed every 0.5s)
    float dispPeakL = -100.0f;
    float dispPeakR = -100.0f;
    float dispPeakHoldL = -100.0f;
    float dispPeakHoldR = -100.0f;
    float dispMomentaryLufs = -14.0f;
    float dispShortTermLufs = -14.0f;
    float dispCrest = 10.0f;
    juce::String dispSpotifyStatus { "-14 TARGET" };
    juce::Colour dispSpotifyCol { 0xff00ffaa };

    void configureKnob(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix = "", juce::Colour accent = juce::Colour(0xff00f0ff));
    void configureBypassButton(juce::ToggleButton& btn);

    void drawSpectrumScreen(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawEQNode(juce::Graphics& g, float x, float y, int bandNum, float gainDb, juce::Colour col);
    void drawGoniometerScreen(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawMetersScreen(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawModuleCard(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title, juce::Colour accentCol = juce::Colour(0xff00f0ff));
    void drawAgentDeck(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawLegendDrawerOverlay(juce::Graphics& g);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomasterSupremeAudioProcessorEditor)
};
