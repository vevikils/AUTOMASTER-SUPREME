#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// Custom Vintage Opto Knob LookAndFeel
class VintageOptoKnobLF : public juce::LookAndFeel_V4
{
public:
    VintageOptoKnobLF()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(0xff222222));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(6.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // 1. Cast Outer Drop Shadow
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillEllipse(rx + 2.0f, ry + 4.0f, rw, rw);

        // 2. Beveled Metal Skirt (Fluted Bakelite Collar)
        juce::ColourGradient skirtGrad(juce::Colour(0xff444444), centreX, ry,
                                     juce::Colour(0xff151515), centreX, ry + rw, false);
        g.setGradientFill(skirtGrad);
        g.fillEllipse(rx, ry, rw, rw);

        // Outer Metallic Rim
        g.setColour(juce::Colour(0xff666666));
        g.drawEllipse(rx, ry, rw, rw, 1.5f);

        // 3. Inner Raised Cap (Matte Bakelite Body)
        float innerInset = radius * 0.22f;
        auto innerBounds = bounds.reduced(innerInset);
        float irx = innerBounds.getX();
        float iry = innerBounds.getY();
        float irw = innerBounds.getWidth();

        juce::ColourGradient capGrad(juce::Colour(0xff282828), centreX, iry,
                                    juce::Colour(0xff0d0d0d), centreX, iry + irw, false);
        g.setGradientFill(capGrad);
        g.fillEllipse(irx, iry, irw, irw);

        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawEllipse(irx, iry, irw, irw, 1.2f);

        // 4. Center Cap / Disc (Spun Brushed Aluminum with Vintage Sheen)
        float centerInset = radius * 0.52f;
        auto centerBounds = bounds.reduced(centerInset);
        juce::ColourGradient centerGrad(juce::Colour(0xff8a8a8a), centerBounds.getX(), centerBounds.getY(),
                                       juce::Colour(0xff3c3c3c), centerBounds.getRight(), centerBounds.getBottom(), false);
        g.setGradientFill(centerGrad);
        g.fillEllipse(centerBounds);

        g.setColour(juce::Colour(0xffaaaaaa));
        g.drawEllipse(centerBounds, 1.0f);

        // 5. White / Phosphor Engraved Pointer Line
        juce::Path p;
        auto pointerLength = radius * 0.76f;
        auto pointerThickness = 3.2f;
        p.addRectangle(-pointerThickness * 0.5f, -radius * 0.92f, pointerThickness, pointerLength * 0.55f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};

// Vintage Warm Amber / VU Meter Component
class VintageVUMeterComponent : public juce::Component
{
public:
    VintageVUMeterComponent()
    {
        setOpaque(false);
    }

    void setLevels(float grDb, float inDb, float outDb, int meterMode)
    {
        targetGr = grDb;
        targetIn = inDb;
        targetOut = outDb;
        mode = meterMode;

        // Ballistics: Standard VU mechanical needle response (~300ms)
        float targetValue = 0.0f;
        if (mode == 0) // GR: 0 dB to -20 dB
        {
            targetValue = juce::jlimit(0.0f, 20.0f, targetGr);
        }
        else if (mode == 1) // +4 Input
        {
            targetValue = juce::jlimit(-20.0f, 3.0f, targetIn + 4.0f);
        }
        else // +10 Output
        {
            targetValue = juce::jlimit(-20.0f, 3.0f, targetOut + 10.0f);
        }

        // Smooth spring physics for needle
        needlePos += 0.22f * (targetValue - needlePos);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // 1. Recessed Outer Bezel (Heavy Black Rim)
        g.setColour(juce::Colour(0xff121212));
        g.fillRoundedRectangle(bounds, 10.0f);

        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 9.0f, 2.0f);

        // 2. Vintage Amber Incandescent Glow Dial Face
        auto dialBounds = bounds.reduced(8.0f);

        // CLIP TO DIAL BOUNDS SO THE NEEDLE NEVER ESCAPES THE GLASS / INNER BEZEL!
        juce::Graphics::ScopedSaveState sss(g);
        g.reduceClipRegion(dialBounds.toNearestInt());

        juce::ColourGradient dialGrad(juce::Colour(0xfff5dfaa), dialBounds.getCentreX(), dialBounds.getY(),
                                     juce::Colour(0xffd8b066), dialBounds.getCentreX(), dialBounds.getBottom(), false);
        g.setGradientFill(dialGrad);
        g.fillRoundedRectangle(dialBounds, 6.0f);

        // Subtle internal shadow inside bezel
        g.setColour(juce::Colours::black.withAlpha(0.20f));
        g.drawRoundedRectangle(dialBounds, 6.0f, 2.0f);

        // 3. Dial Arc Scale Serigraphy
        float cx = dialBounds.getCentreX();
        float pivotY = dialBounds.getBottom() + dialBounds.getHeight() * 0.70f;
        float needleRadius = dialBounds.getHeight() * 1.38f;

        g.setFont(juce::Font("Georgia", 11.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xff2a1e12));

        // VU Header Text
        g.drawText("VU", cx - 25, dialBounds.getY() + 14, 50, 16, juce::Justification::centred);
        g.setFont(juce::Font("Helvetica", 9.0f, juce::Font::bold));
        g.drawText(mode == 0 ? "GAIN REDUCTION (dB)" : (mode == 1 ? "+4 INPUT MONITOR" : "+10 OUTPUT MONITOR"),
                   cx - 90, dialBounds.getY() + 30, 180, 14, juce::Justification::centred);

        // Draw Arc Scale Ticks (0 on right for GR, or 0 on middle-right)
        for (int i = 0; i <= 20; i += 2)
        {
            float norm = (float)i / 20.0f;
            float angleDeg = (mode == 0)
                ? (35.0f - norm * 70.0f) // GR: 0 on right (35 deg), 20 on left (-35 deg)
                : (-35.0f + norm * 70.0f);

            float rad = juce::degreesToRadians(angleDeg);
            float x1 = cx + needleRadius * 0.88f * std::sin(rad);
            float y1 = pivotY - needleRadius * 0.88f * std::cos(rad);
            float x2 = cx + needleRadius * 0.98f * std::sin(rad);
            float y2 = pivotY - needleRadius * 0.98f * std::cos(rad);

            g.drawLine(x1, y1, x2, y2, (i % 5 == 0) ? 2.0f : 1.0f);

            if (i % 5 == 0 && (i <= 20))
            {
                juce::String label = (mode == 0) ? juce::String(-i) : juce::String(i - 20);
                if (mode == 0 && i == 0) label = "0";
                g.drawText(label, (int)x1 - 12, (int)y1 - 14, 24, 12, juce::Justification::centred);
            }
        }

        // Red Zone on right (0 to +3 dB)
        g.setColour(juce::Colour(0xffc5221f));
        g.drawText("+1 +2 +3", dialBounds.getRight() - 58, dialBounds.getY() + 48, 50, 14, juce::Justification::centredRight);

        // 4. Physical Moving Needle (Kept inside meter face)
        float needleNorm = (mode == 0)
            ? (needlePos / 20.0f)
            : ((needlePos + 20.0f) / 23.0f);

        float needleAngleDeg = (mode == 0)
            ? (35.0f - needleNorm * 70.0f)
            : (-35.0f + needleNorm * 70.0f);

        needleAngleDeg = juce::jlimit(-40.0f, 40.0f, needleAngleDeg);
        float needleRad = juce::degreesToRadians(needleAngleDeg);

        float tipX = cx + needleRadius * std::sin(needleRad);
        float tipY = pivotY - needleRadius * std::cos(needleRad);

        // Shadow under needle
        g.setColour(juce::Colours::black.withAlpha(0.28f));
        g.drawLine(cx + 3.0f, pivotY + 2.0f, tipX + 3.0f, tipY + 2.0f, 1.8f);

        // Needle body
        g.setColour(juce::Colour(0xff181410));
        g.drawLine(cx, pivotY, tipX, tipY, 1.6f);

        // Red accent tip
        g.setColour(juce::Colour(0xffcc1111));
        float midTipX = cx + needleRadius * 0.90f * std::sin(needleRad);
        float midTipY = pivotY - needleRadius * 0.90f * std::cos(needleRad);
        g.drawLine(midTipX, midTipY, tipX, tipY, 2.2f);

        // 5. Plastic / Acrylic Glass Reflection Sheen
        juce::Path glassPath;
        glassPath.startNewSubPath(dialBounds.getX(), dialBounds.getY());
        glassPath.lineTo(dialBounds.getRight(), dialBounds.getY());
        glassPath.lineTo(dialBounds.getRight(), dialBounds.getY() + dialBounds.getHeight() * 0.45f);
        glassPath.lineTo(dialBounds.getX(), dialBounds.getY() + dialBounds.getHeight() * 0.20f);
        glassPath.closeSubPath();

        g.setColour(juce::Colours::white.withAlpha(0.12f));
        g.fillPath(glassPath);
    }

private:
    float needlePos = 0.0f;
    float targetGr = 0.0f;
    float targetIn = -60.0f;
    float targetOut = -60.0f;
    int mode = 0;
};

class SupremeVocalBusCompressorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                     public juce::Timer
{
public:
    SupremeVocalBusCompressorAudioProcessorEditor(SupremeVocalBusCompressorAudioProcessor&);
    ~SupremeVocalBusCompressorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SupremeVocalBusCompressorAudioProcessor& audioProcessor;

    VintageOptoKnobLF knobLF;
    VintageVUMeterComponent vuMeter;

    // Main Knobs
    juce::Slider peakReductionSlider;
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakReductionAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttach;

    // Special Feature Knobs (Harmonic Sheen & Tube Saturation)
    juce::Slider sheenAmountSlider;
    juce::Slider sheenFreqSlider;
    juce::Slider tubeWarmthSlider;
    juce::Slider hfEmphasisSlider;
    juce::Slider dryWetSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sheenAmountAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sheenFreqAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tubeWarmthAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hfEmphasisAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttach;

    // Toggle Mode (Compress / Limit)
    juce::ToggleButton modeToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modeAttach;

    // Meter selector (GR, +4, +10)
    juce::ComboBox meterModeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> meterModeAttach;

    // Presets Combo Box
    juce::ComboBox presetBox;

    void configureKnob(juce::Slider& s, const juce::String& text, const juce::String& suffix = "");

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SupremeVocalBusCompressorAudioProcessorEditor)
};
