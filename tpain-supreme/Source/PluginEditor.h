#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

//==============================================================================
// Supreme Waves Look & Feel: Luxury Dark Studio + Neon Violet & Hot Pink
//==============================================================================
class SupremeWavesLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SupremeWavesLookAndFeel();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;
};

//==============================================================================
// Main Editor Component: Antares Auto-Tune Pitch Wheel + Waves Aesthetic
//==============================================================================
class TPainSupremeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit TPainSupremeAudioProcessorEditor(TPainSupremeAudioProcessor&);
    ~TPainSupremeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    TPainSupremeAudioProcessor& audioProcessor;
    SupremeWavesLookAndFeel wavesLookAndFeel;

    // Parameter Controls
    juce::ComboBox presetSelector;
    juce::Label   presetLabel;

    juce::ComboBox rootSelector;
    juce::ComboBox scaleSelector;

    juce::Slider retuneSpeedSlider;
    juce::Slider transitionSlider;
    juce::Slider variationSlider;
    juce::Slider centeringSlider;

    // Dynamics Knobs (Transients & Compression Rotary Knobs)
    juce::Slider transientsSlider;
    juce::Slider compressionSlider;
    juce::Slider lowCutSlider;
    juce::Slider bodySlider;
    juce::Slider airSlider;

    juce::Slider reverbMixSlider;
    juce::Slider reverbSizeSlider;
    juce::Slider delayMixSlider;
    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;

    // Gain Staging & Master Controls (Input Gain, Stereo Width, Mix Knobs + Golden Master Fader)
    juce::Slider inputGainSlider;
    juce::Slider stereoWidthSlider;
    juce::Slider outputSlider;
    juce::Slider mixSlider;

    // Noise Gate knobs (FL Studio Rotary)
    juce::Slider gateThreshSlider;
    juce::Slider gateAttackSlider;
    juce::Slider gateReleaseSlider;

    // Attachments
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<ComboBoxAttachment> rootAttachment;
    std::unique_ptr<ComboBoxAttachment> scaleAttachment;

    std::unique_ptr<SliderAttachment> retuneSpeedAttachment;
    std::unique_ptr<SliderAttachment> transitionAttachment;
    std::unique_ptr<SliderAttachment> variationAttachment;
    std::unique_ptr<SliderAttachment> centeringAttachment;

    std::unique_ptr<SliderAttachment> transientsAttachment;
    std::unique_ptr<SliderAttachment> compressionAttachment;
    std::unique_ptr<SliderAttachment> lowCutAttachment;
    std::unique_ptr<SliderAttachment> bodyAttachment;
    std::unique_ptr<SliderAttachment> airAttachment;

    std::unique_ptr<SliderAttachment> reverbMixAttachment;
    std::unique_ptr<SliderAttachment> reverbSizeAttachment;
    std::unique_ptr<SliderAttachment> delayMixAttachment;
    std::unique_ptr<SliderAttachment> delayTimeAttachment;
    std::unique_ptr<SliderAttachment> delayFeedbackAttachment;

    std::unique_ptr<SliderAttachment> inputGainAttachment;
    std::unique_ptr<SliderAttachment> stereoWidthAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;

    std::unique_ptr<SliderAttachment> gateThreshAttachment;
    std::unique_ptr<SliderAttachment> gateAttackAttachment;
    std::unique_ptr<SliderAttachment> gateReleaseAttachment;

    // Labels
    juce::Label rootLabel;
    juce::Label scaleLabel;
    juce::Label speedLabel;
    juce::Label transSpeedLabel;
    juce::Label varLabel;
    juce::Label centerLabel;

    juce::Label transLabel;
    juce::Label compLabel;
    juce::Label lowCutLabel;
    juce::Label bodyLabel;
    juce::Label airLabel;

    juce::Label revMixLabel;
    juce::Label revSizeLabel;
    juce::Label dlyMixLabel;
    juce::Label dlyTimeLabel;
    juce::Label dlyFbLabel;

    juce::Label inputGainLabel;
    juce::Label stereoWidthLabel;
    juce::Label outLabel;
    juce::Label mixLabel;

    juce::Label gateThrLabel;
    juce::Label gateAtkLabel;
    juce::Label gateRelLabel;

    // Real-Time Cached Telemetry
    juce::String currentDetectedNote = "--";
    juce::String currentTargetNote = "--";
    float currentCentsDeviation = 0.0f;
    float currentHz = 0.0f;
    bool isVoiced = false;
    float confidence = 0.0f;
    float currentGR = 0.0f;
    float currentTransient = 0.0f;
    float currentGateGR = 0.0f;
    bool isGateOpen = true;
    float currentInputMeterDb = -60.0f;
    float currentOutputMeterDb = -60.0f;

    int currentRootIndex = 0;
    int currentScaleIndex = 0;
    float smoothedPitchAngle = 0.0f;

    void configureKnob(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix = "");
    void configureVerticalFader(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix = "");
    void drawAntaresPitchWheel(juce::Graphics& g, juce::Rectangle<float> area);
    void drawWavesRackUnit(juce::Graphics& g, juce::Rectangle<float> bounds, const juce::String& title, const juce::Colour& headerGlow);
    void drawLivePeakMeter(juce::Graphics& g, juce::Rectangle<float> meterArea, float levelDb, const juce::String& title);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TPainSupremeAudioProcessorEditor)
};
