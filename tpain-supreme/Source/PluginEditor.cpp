#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BgTextureData.h"

//==============================================================================
// Supreme Waves Look & Feel: Hybrid Edition (Fruity Limiter Knobs + 24K Gold Fader)
//==============================================================================

SupremeWavesLookAndFeel::SupremeWavesLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff060c14));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xdd0e0a20));
    setColour(juce::PopupMenu::textColourId, juce::Colour(0xfff0eaff));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffff007f).withAlpha(0.28f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

void SupremeWavesLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPosProportional, float rotaryStartAngle,
                                              float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    auto currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    bool isHero = slider.getName() == "RetuneSpeed";
    bool isMaster = slider.getProperties().contains("isMasterZone") && (bool)slider.getProperties()["isMasterZone"];

    // Palette: Master zone preserves amber/gold (#f59e0b, #fbbf24).
    // All other circular knobs across the plugin are vibrant neon violet (#a855f7, #d8b4fe).
    juce::Colour haloColor    = isMaster ? juce::Colour(0xfff59e0b) : juce::Colour(0xffa855f7);
    juce::Colour primaryColor = isMaster ? juce::Colour(0xfff59e0b) : juce::Colour(0xffa855f7);
    juce::Colour brightColor  = isMaster ? juce::Colour(0xfffbbf24) : juce::Colour(0xffd8b4fe);
    juce::Colour rimHighlight = isMaster ? juce::Colour(0xff4b5563) : juce::Colour(0xff6b46c1);

    // 1. Ambient glow halo
    float haloR = radius + (isHero ? 12.0f : 8.0f);
    juce::ColourGradient halo(
        haloColor.withAlpha(isHero ? 0.32f : 0.18f),
        centre.x, centre.y,
        juce::Colours::transparentBlack,
        centre.x + haloR, centre.y + haloR, true);
    g.setGradientFill(halo);
    g.fillEllipse(centre.x - haloR, centre.y - haloR, haloR * 2.0f, haloR * 2.0f);

    // 2. Dark glass track groove
    float trackWidth = isHero ? 6.5f : 4.5f;
    float arcR = radius - trackWidth * 0.5f - 1.5f;

    juce::Path bgArc;
    bgArc.addCentredArc(centre.x, centre.y, arcR, arcR, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff12151b));
    g.strokePath(bgArc, juce::PathStrokeType(trackWidth + 1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour(juce::Colour(0xff090b0e));
    g.strokePath(bgArc, juce::PathStrokeType(trackWidth - 1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Active Vibrant Value Arc (FL Amber for Master, Neon Violet for all other units)
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, arcR, arcR, 0.0f, rotaryStartAngle, currentAngle, true);

    // Soft neon glow pass
    g.setColour(brightColor.withAlpha(0.40f));
    g.strokePath(valueArc, juce::PathStrokeType(trackWidth + 2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Sharp gradient arc
    juce::ColourGradient arcGrad(brightColor, centre.x - radius, centre.y + radius,
                                 primaryColor, centre.x + radius, centre.y - radius, false);
    g.setGradientFill(arcGrad);
    g.strokePath(valueArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 4. Dark Carbon / Slate Knob Cap Disc
    float capRadius = arcR - trackWidth * 0.5f - (isHero ? 4.5f : 3.0f);
    if (capRadius > 2.0f)
    {
        auto capX = centre.x - capRadius;
        auto capY = centre.y - capRadius;
        auto capD = capRadius * 2.0f;

        // Base gradient: dark carbon disc
        juce::Colour capTop = isMaster ? juce::Colour(0xff262c36) : juce::Colour(0xff281c3a);
        juce::Colour capBot = isMaster ? juce::Colour(0xff13171d) : juce::Colour(0xff120a1f);
        juce::ColourGradient capBase(
            capTop, centre.x - capRadius * 0.45f, centre.y - capRadius * 0.55f,
            capBot, centre.x + capRadius * 0.55f, centre.y + capRadius * 0.65f, true);
        g.setGradientFill(capBase);
        g.fillEllipse(capX, capY, capD, capD);

        // Soft specular sheen on upper-left quadrant
        juce::ColourGradient capSheen(
            juce::Colour(0xffffffff).withAlpha(isHero ? 0.22f : 0.16f),
            centre.x - capRadius * 0.35f, centre.y - capRadius * 0.50f,
            juce::Colours::transparentWhite,
            centre.x + capRadius * 0.20f, centre.y + capRadius * 0.10f, true);
        g.setGradientFill(capSheen);
        g.fillEllipse(capX, capY, capD, capD);

        // Outer beveled rim
        juce::Colour rimBot = isMaster ? juce::Colour(0xff1b2027) : juce::Colour(0xff1f1230);
        juce::ColourGradient rimGrad(
            rimHighlight, centre.x - capRadius, centre.y - capRadius,
            rimBot, centre.x + capRadius, centre.y + capRadius, false);
        g.setGradientFill(rimGrad);
        g.drawEllipse(capX, capY, capD, capD, 1.2f);

        // Inner ridge
        g.setColour(isMaster ? juce::Colour(0xff1f252e).withAlpha(0.8f) : juce::Colour(0xff2a1842).withAlpha(0.8f));
        g.drawEllipse(capX + 1.2f, capY + 1.2f, capD - 2.4f, capD - 2.4f, 0.8f);

        // 5. Glowing Pointer Needle with White Core
        float r1 = capRadius * 0.32f;
        float r2 = capRadius * 0.86f;
        float sinA = std::sin(currentAngle);
        float cosA = -std::cos(currentAngle); // 0 at 12 o'clock

        juce::Line<float> needle(centre.x + r1 * sinA, centre.y + r1 * cosA,
                                 centre.x + r2 * sinA, centre.y + r2 * cosA);

        // Glow pass
        g.setColour(brightColor.withAlpha(0.65f));
        g.drawLine(needle, isHero ? 4.0f : 3.2f);

        // Sharp core needle
        g.setColour(juce::Colours::white);
        g.drawLine(needle, isHero ? 2.0f : 1.6f);

        // Center jewel orb
        g.setColour(primaryColor);
        g.fillEllipse(centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.fillEllipse(centre.x - 1.0f, centre.y - 1.0f, 2.0f, 2.0f);
    }
}

void SupremeWavesLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                              const juce::Slider::SliderStyle /*style*/, juce::Slider& slider)
{
    float cx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
    bool isMaster = slider.getProperties().contains("isMasterZone") && (bool)slider.getProperties()["isMasterZone"];

    // Solid Console Fader Handle
    float thumbW = 42.0f;
    float thumbH = 22.0f;
    float thumbY = sliderPos - thumbH * 0.5f;
    auto thumbRect = juce::Rectangle<float>(cx - thumbW * 0.5f, thumbY, thumbW, thumbH);

    // Soft drop shadow cast on plate
    g.setColour(juce::Colour(0x66000000));
    g.fillRoundedRectangle(thumbRect.translated(0.0f, 2.5f), 3.0f);

    if (isMaster)
    {
        // 24K Brushed Gold metallic gradient
        juce::ColourGradient goldCap(
            juce::Colour(0xfffff2a3), thumbRect.getX(), thumbRect.getY(),
            juce::Colour(0xff7a520a), thumbRect.getX(), thumbRect.getBottom(), false);
        goldCap.addColour(0.35, juce::Colour(0xffffd700));
        goldCap.addColour(0.70, juce::Colour(0xffb8860b));
        g.setGradientFill(goldCap);
        g.fillRoundedRectangle(thumbRect, 2.5f);

        // Top specular highlight & dark bevel
        g.setColour(juce::Colour(0xfffffbd4));
        g.drawHorizontalLine(static_cast<int>(thumbRect.getY()), thumbRect.getX() + 1.0f, thumbRect.getRight() - 1.0f);
        g.setColour(juce::Colour(0xff573a06));
        g.drawHorizontalLine(static_cast<int>(thumbRect.getBottom() - 1.0f), thumbRect.getX() + 1.0f, thumbRect.getRight() - 1.0f);
        g.drawRoundedRectangle(thumbRect, 2.5f, 1.0f);

        // Tactile knurled finger ridges
        g.setColour(juce::Colour(0xff5c3e06));
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 4.0f, thumbRect.getWidth() - 8.0f, 1.0f);
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 16.0f, thumbRect.getWidth() - 8.0f, 1.0f);
        g.setColour(juce::Colour(0xffffe680));
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 5.0f, thumbRect.getWidth() - 8.0f, 0.5f);
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 17.0f, thumbRect.getWidth() - 8.0f, 0.5f);

        // Illuminated Horizontal Center Notch Line (Bright White Core with Gold Halo)
        g.setColour(juce::Colour(0xffffd700).withAlpha(0.65f));
        g.fillRect(thumbRect.getX() + 2.0f, thumbY + 9.5f, thumbRect.getWidth() - 4.0f, 2.5f);
        g.setColour(juce::Colours::white);
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 10.2f, thumbRect.getWidth() - 8.0f, 1.2f);
    }
    else
    {
        // Dynamics Fader Cap: Two-Tone Brushed Obsidian & Gold with Neon Violet Center Notch
        juce::ColourGradient dynCap(
            juce::Colour(0xfffff099), thumbRect.getX(), thumbRect.getY(),
            juce::Colour(0xff573a06), thumbRect.getX(), thumbRect.getBottom(), false);
        dynCap.addColour(0.35, juce::Colour(0xffd4af37));
        dynCap.addColour(0.70, juce::Colour(0xff8c6214));
        g.setGradientFill(dynCap);
        g.fillRoundedRectangle(thumbRect, 2.5f);

        // Top specular highlight & dark bevel
        g.setColour(juce::Colour(0xfffffbd4));
        g.drawHorizontalLine(static_cast<int>(thumbRect.getY()), thumbRect.getX() + 1.0f, thumbRect.getRight() - 1.0f);
        g.setColour(juce::Colour(0xff2d1748));
        g.drawHorizontalLine(static_cast<int>(thumbRect.getBottom() - 1.0f), thumbRect.getX() + 1.0f, thumbRect.getRight() - 1.0f);
        g.drawRoundedRectangle(thumbRect, 2.5f, 1.0f);

        // Tactile knurled finger ridges with subtle violet accent
        g.setColour(juce::Colour(0xff3b1f5c));
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 4.0f, thumbRect.getWidth() - 8.0f, 1.0f);
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 16.0f, thumbRect.getWidth() - 8.0f, 1.0f);
        g.setColour(juce::Colour(0xffffe680));
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 5.0f, thumbRect.getWidth() - 8.0f, 0.5f);
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 17.0f, thumbRect.getWidth() - 8.0f, 0.5f);

        // Illuminated Horizontal Center Notch Line (Bright White Core with Neon Violet Halo)
        g.setColour(juce::Colour(0xffa855f7).withAlpha(0.75f));
        g.fillRect(thumbRect.getX() + 2.0f, thumbY + 9.5f, thumbRect.getWidth() - 4.0f, 2.5f);
        g.setColour(juce::Colours::white);
        g.fillRect(thumbRect.getX() + 4.0f, thumbY + 10.2f, thumbRect.getWidth() - 8.0f, 1.2f);
    }
}

void SupremeWavesLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                          int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                          juce::ComboBox& box)
{
    bool focused = box.hasKeyboardFocus(false) || isButtonDown;
    auto area = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f);
    float cr = 8.0f;

    juce::ColourGradient base(
        juce::Colour(0xff160d2c).withAlpha(0.88f), 0.0f, 0.0f,
        juce::Colour(0xff0a0618).withAlpha(0.82f), 0.0f, (float)height, false);
    g.setGradientFill(base);
    g.fillRoundedRectangle(area, cr);

    juce::ColourGradient sheen(
        juce::Colour(0xffffffff).withAlpha(0.09f), 0.0f, 0.0f,
        juce::Colours::transparentWhite, 0.0f, height * 0.45f, false);
    g.setGradientFill(sheen);
    g.fillRoundedRectangle(area.withHeight(height * 0.45f), cr);

    g.setColour(focused ? juce::Colour(0xffff007f).withAlpha(0.80f) : juce::Colour(0xffffffff).withAlpha(0.18f));
    g.drawRoundedRectangle(area, cr, 1.1f);

    juce::Path arrow;
    float ax = width - 18.0f, ay = height * 0.5f - 2.5f;
    arrow.startNewSubPath(ax, ay);
    arrow.lineTo(ax + 5.0f, ay + 5.0f);
    arrow.lineTo(ax + 10.0f, ay);
    g.setColour(focused ? juce::Colour(0xffff007f) : juce::Colour(0xffc77dff));
    g.strokePath(arrow, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

//==============================================================================
// TPainSupremeAudioProcessorEditor Implementation (v3.6 Hybrid Edition)
//==============================================================================

TPainSupremeAudioProcessorEditor::TPainSupremeAudioProcessorEditor(TPainSupremeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&wavesLookAndFeel);
    bgTextureImage = juce::ImageFileFormat::loadFrom(BgTexture::data, BgTexture::dataSize);

    // ---- Artist Preset Selector ----
    const auto& presets = TPainSupremeAudioProcessor::getPresets();
    for (int i = 0; i < (int)presets.size(); ++i)
        presetSelector.addItem(presets[i].name, i + 1);
    presetSelector.setSelectedId(1, juce::dontSendNotification);
    presetSelector.onChange = [this]
    {
        int idx = presetSelector.getSelectedItemIndex();
        if (idx >= 0)
            audioProcessor.setCurrentProgram(idx);
    };
    presetLabel.setText("ARTIST PRESET", juce::dontSendNotification);
    presetLabel.setFont(juce::Font(11.5f, juce::Font::bold));
    presetLabel.setJustificationType(juce::Justification::centred);
    presetLabel.setColour(juce::Label::textColourId, juce::Colour(0xffc77dff));
    addAndMakeVisible(presetSelector);
    addAndMakeVisible(presetLabel);

    // Populate Root Notes
    for (int i = 0; i < 12; ++i)
        rootSelector.addItem(VeviAudio::NoteNames[i], i + 1);

    // Populate Scales
    scaleSelector.addItem("Major", 1);
    scaleSelector.addItem("Minor (Natural)", 2);
    scaleSelector.addItem("Harmonic Minor", 3);
    scaleSelector.addItem("Melodic Minor", 4);
    scaleSelector.addItem("Pentatonic Major", 5);
    scaleSelector.addItem("Pentatonic Minor", 6);
    scaleSelector.addItem("Blues", 7);
    scaleSelector.addItem("Chromatic", 8);

    addAndMakeVisible(rootSelector);
    addAndMakeVisible(scaleSelector);

    // Attachments for Selectors
    rootAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_ROOT, rootSelector);
    scaleAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_SCALE, scaleSelector);

    // Configure Tuning Knobs
    configureKnob(retuneSpeedSlider, speedLabel, "RETUNE SPEED", " ms");
    retuneSpeedSlider.setName("RetuneSpeed");

    configureKnob(transitionSlider, transSpeedLabel, "TRANSITION", " ms");
    configureKnob(variationSlider, varLabel, "VARIATION", " %");
    configureKnob(centeringSlider, centerLabel, "CENTERING", " %");

    // Configure Dynamics: Dual Vertical Faders
    configureVerticalFader(transientsSlider, transLabel, "TRANSIENTS", " %");
    configureVerticalFader(compressionSlider, compLabel, "COMPRESSION", " %");

    // Configure Vocal Tone Knobs
    configureKnob(lowCutSlider, lowCutLabel, "LOW CUT", " Hz");
    configureKnob(bodySlider, bodyLabel, "BODY", " dB");
    configureKnob(airSlider, airLabel, "AIR SHEEN", " dB");

    // Configure Space FX Knobs
    configureKnob(reverbMixSlider, revMixLabel, "REV MIX", " %");
    configureKnob(reverbSizeSlider, revSizeLabel, "REV SIZE", " %");
    configureKnob(delayMixSlider, dlyMixLabel, "DLY MIX", " %");
    configureKnob(delayTimeSlider, dlyTimeLabel, "DLY TIME", " ms");
    configureKnob(delayFeedbackSlider, dlyFbLabel, "FEEDBACK", " %");

    // Configure Gain Staging & Master (Stereo Width, Mix Knobs + 24K Gold Master Fader)
    configureKnob(stereoWidthSlider, stereoWidthLabel, "STEREO WIDTH", " %");
    configureKnob(mixSlider, mixLabel, "DRY / WET", " %");
    stereoWidthSlider.getProperties().set("isMasterZone", true);
    mixSlider.getProperties().set("isMasterZone", true);

    configureVerticalFader(outputSlider, outLabel, "MASTER LEVEL", " dB");
    outputSlider.setRange(-24.0, 6.0, 0.1);

    // Configure Noise Gate Knobs
    configureKnob(gateThreshSlider, gateThrLabel, "UMBRAL", " dB");
    configureKnob(gateAttackSlider, gateAtkLabel, "ATAQUE", " ms");
    configureKnob(gateReleaseSlider, gateRelLabel, "LIBERACION", " ms");

    // Parameter Attachments
    retuneSpeedAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_RETUNE_SPEED, retuneSpeedSlider);
    transitionAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_TRANSITION, transitionSlider);
    variationAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_VARIATION, variationSlider);
    centeringAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_CENTERING, centeringSlider);

    transientsAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_TRANSIENTS, transientsSlider);
    compressionAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_COMPRESSION, compressionSlider);
    lowCutAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_LOW_CUT, lowCutSlider);
    bodyAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_BODY, bodySlider);
    airAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_AIR, airSlider);

    reverbMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_REVERB_MIX, reverbMixSlider);
    reverbSizeAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_REVERB_SIZE, reverbSizeSlider);
    delayMixAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_DELAY_MIX, delayMixSlider);
    delayTimeAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_DELAY_TIME, delayTimeSlider);
    delayFeedbackAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_DELAY_FEEDBACK, delayFeedbackSlider);

    inputGainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_INPUT, inputGainSlider);
    stereoWidthAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_STEREO_WIDTH, stereoWidthSlider);
    outputAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_OUTPUT, outputSlider);
    mixAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_MIX, mixSlider);

    gateThreshAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_GATE_THRESH, gateThreshSlider);
    gateAttackAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_GATE_ATTACK, gateAttackSlider);
    gateReleaseAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, TPainSupremeAudioProcessor::ID_GATE_RELEASE, gateReleaseSlider);

    // Full Studio Waves Modular Layout Size
    setSize(1250, 840);
    startTimerHz(30);
}

TPainSupremeAudioProcessorEditor::~TPainSupremeAudioProcessorEditor()
{
    stopTimer();

    rootAttachment.reset();
    scaleAttachment.reset();
    retuneSpeedAttachment.reset();
    transitionAttachment.reset();
    variationAttachment.reset();
    centeringAttachment.reset();
    transientsAttachment.reset();
    compressionAttachment.reset();
    lowCutAttachment.reset();
    bodyAttachment.reset();
    airAttachment.reset();
    reverbMixAttachment.reset();
    reverbSizeAttachment.reset();
    delayMixAttachment.reset();
    delayTimeAttachment.reset();
    delayFeedbackAttachment.reset();
    inputGainAttachment.reset();
    stereoWidthAttachment.reset();
    outputAttachment.reset();
    mixAttachment.reset();
    gateThreshAttachment.reset();
    gateAttackAttachment.reset();
    gateReleaseAttachment.reset();

    setLookAndFeel(nullptr);
}

void TPainSupremeAudioProcessorEditor::configureKnob(juce::Slider& slider, juce::Label& label,
                                                    const juce::String& text, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 68, 16);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff0e8ff));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.5f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffc77dff));
    addAndMakeVisible(label);
}

void TPainSupremeAudioProcessorEditor::configureVerticalFader(juce::Slider& slider, juce::Label& label,
                                                             const juce::String& text, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 16);
    slider.setTextValueSuffix(suffix);
    bool isMaster = slider.getProperties().contains("isMasterZone") && (bool)slider.getProperties()["isMasterZone"];
    slider.setColour(juce::Slider::textBoxTextColourId, isMaster ? juce::Colour(0xffffd700) : juce::Colour(0xffe9d5ff));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.5f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffc77dff));
    addAndMakeVisible(label);
}

void TPainSupremeAudioProcessorEditor::drawLivePeakMeter(juce::Graphics& g, juce::Rectangle<float> meterArea,
                                                       float levelDb, const juce::String& /*title*/)
{
    g.setColour(juce::Colour(0xff0d0914));
    g.fillRoundedRectangle(meterArea, 3.0f);
    g.setColour(juce::Colour(0xff6e511b));
    g.drawRoundedRectangle(meterArea, 3.0f, 1.0f);

    float clampedDb = juce::jlimit(-60.0f, 6.0f, levelDb);
    float norm = (clampedDb + 60.0f) / 66.0f;
    float activeH = norm * (meterArea.getHeight() - 4.0f);

    if (activeH > 1.0f)
    {
        auto activeRect = juce::Rectangle<float>(meterArea.getX() + 2.0f, meterArea.getBottom() - 2.0f - activeH,
                                                meterArea.getWidth() - 4.0f, activeH);
        g.setColour(juce::Colour(0xff00f5ff));
        g.fillRoundedRectangle(activeRect, 2.0f);
    }
}

void TPainSupremeAudioProcessorEditor::timerCallback()
{
    currentDetectedNote = audioProcessor.getDetectedNoteName();
    currentTargetNote = audioProcessor.getTargetNoteName();
    currentCentsDeviation = audioProcessor.getCentsDeviation();
    currentHz = audioProcessor.getDetectedPitchHz();
    isVoiced = audioProcessor.getIsVoiced();
    confidence = audioProcessor.getConfidence();

    currentGR = audioProcessor.getGainReductionDb();
    currentTransient = audioProcessor.getTransientActivity();
    currentGateGR = audioProcessor.getGateGainReductionDb();
    isGateOpen = audioProcessor.getIsGateOpen();

    currentInputMeterDb = audioProcessor.getInputMeterPeakDb();
    currentOutputMeterDb = audioProcessor.getOutputMeterPeakDb();

    // Smooth needle dynamics (300 ms integration VU ballistics)
    float targetLeftNorm = juce::jlimit(0.0f, 1.0f, (currentInputMeterDb + 24.0f) / 27.0f);
    float targetRightNorm = juce::jlimit(0.0f, 1.0f, (currentOutputMeterDb + 24.0f) / 27.0f);

    float attackCoeff = 0.22f;
    float releaseCoeff = 0.12f;

    smoothedLeftVU += (targetLeftNorm > smoothedLeftVU ? attackCoeff : releaseCoeff) * (targetLeftNorm - smoothedLeftVU);
    smoothedRightVU += (targetRightNorm > smoothedRightVU ? attackCoeff : releaseCoeff) * (targetRightNorm - smoothedRightVU);

    currentRootIndex = rootSelector.getSelectedId() - 1;
    if (currentRootIndex < 0 || currentRootIndex > 11) currentRootIndex = 0;
    currentScaleIndex = scaleSelector.getSelectedId() - 1;
    if (currentScaleIndex < 0 || currentScaleIndex > 7) currentScaleIndex = 0;

    if (isVoiced && currentHz > 50.0f)
    {
        int detectedMidi = audioProcessor.getDetectedMidiNote();
        int semitone = (detectedMidi % 12 + 12) % 12;
        float continuousSemitone = static_cast<float>(semitone) - (currentCentsDeviation / 100.0f);
        float targetAngle = (continuousSemitone / 12.0f) * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
        smoothedPitchAngle += 0.35f * (targetAngle - smoothedPitchAngle);
    }

    repaint();
}

//==============================================================================
// Analog VU Meters (Los Búmetros) for Unit 4 (Gain Staging & Master)
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawAnalogVUMeter(juce::Graphics& g, juce::Rectangle<float> meterBox, float needleNorm, const juce::String& title)
{
    float cr = 4.0f;

    g.setColour(juce::Colour(0xff0a0712));
    g.fillRoundedRectangle(meterBox, cr);
    g.setColour(juce::Colour(0xff2d2238));
    g.drawRoundedRectangle(meterBox, cr, 0.8f);

    auto dialBox = meterBox.reduced(2.0f);

    // Warm incandescent dial face
    juce::ColourGradient dialFace(
        juce::Colour(0xff301807), dialBox.getCentreX(), dialBox.getY(),
        juce::Colour(0xff140903), dialBox.getCentreX(), dialBox.getBottom(), false);
    g.setGradientFill(dialFace);
    g.fillRoundedRectangle(dialBox, cr - 1.0f);

    // Amber backlight glow
    juce::ColourGradient amberGlow(
        juce::Colour(0xffff8c00).withAlpha(0.28f), dialBox.getCentreX(), dialBox.getY() + 4.0f,
        juce::Colours::transparentBlack, dialBox.getCentreX(), dialBox.getY() + 38.0f, true);
    g.setGradientFill(amberGlow);
    g.fillRoundedRectangle(dialBox, cr - 1.0f);

    float pivotX = dialBox.getCentreX();
    float pivotY = dialBox.getBottom() + 10.0f;
    float arcRadius = 52.0f;

    float startAng = -45.0f * juce::MathConstants<float>::pi / 180.0f;
    float zeroAng = 12.0f * juce::MathConstants<float>::pi / 180.0f;
    float endAng = 38.0f * juce::MathConstants<float>::pi / 180.0f;

    // Normal zone (-20 to 0)
    juce::Path normalArc;
    normalArc.addCentredArc(pivotX, pivotY, arcRadius, arcRadius, 0.0f, startAng, zeroAng, true);
    g.setColour(juce::Colour(0xffffe2b3));
    g.strokePath(normalArc, juce::PathStrokeType(1.2f));

    // Overload red zone (0 to +3 dB)
    juce::Path redArc;
    redArc.addCentredArc(pivotX, pivotY, arcRadius, arcRadius, 0.0f, zeroAng, endAng, true);
    g.setColour(juce::Colour(0xffef4444));
    g.strokePath(redArc, juce::PathStrokeType(1.8f));

    // Dial text
    g.setFont(juce::Font(7.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffe2b3).withAlpha(0.85f));
    g.drawText(title, (int)dialBox.getX(), (int)dialBox.getY() + 20, (int)dialBox.getWidth(), 12, juce::Justification::centred);

    // Dynamic Needle
    float clampedNorm = juce::jlimit(0.0f, 1.0f, needleNorm);
    float needleAngle = startAng + clampedNorm * (endAng - startAng);
    float needleLen = arcRadius + 2.0f;

    float tipX = pivotX + needleLen * std::sin(needleAngle);
    float tipY = pivotY - needleLen * std::cos(needleAngle);

    g.setColour(juce::Colour(0x55000000));
    g.drawLine(pivotX + 1.0f, pivotY + 1.0f, tipX + 1.0f, tipY + 1.0f, 1.8f);

    g.setColour(juce::Colour(0xffff2222));
    g.drawLine(pivotX, pivotY, tipX, tipY, 1.4f);
    g.setColour(juce::Colour(0xffff8888));
    g.drawLine(pivotX, pivotY, tipX, tipY, 0.6f);

    g.setColour(juce::Colour(0xff120c18));
    g.fillEllipse(pivotX - 4.0f, pivotY - 4.0f, 8.0f, 8.0f);
    g.setColour(juce::Colour(0xffd4af37));
    g.drawEllipse(pivotX - 4.0f, pivotY - 4.0f, 8.0f, 8.0f, 0.8f);
}

void TPainSupremeAudioProcessorEditor::drawDualAnalogVUMeters(juce::Graphics& g, juce::Rectangle<float> area)
{
    // Outer acrylic display window
    g.setColour(juce::Colour(0xff090610));
    g.fillRoundedRectangle(area, 6.0f);

    juce::ColourGradient frameGrad(
        juce::Colour(0xff3b2d4c), area.getX(), area.getY(),
        juce::Colour(0xff120d1c), area.getRight(), area.getBottom(), false);
    g.setGradientFill(frameGrad);
    g.drawRoundedRectangle(area, 6.0f, 1.2f);

    float meterW = 112.0f;
    float meterH = 78.0f;

    auto lMeterBox = juce::Rectangle<float>(area.getX() + 6.0f, area.getY() + 6.0f, meterW, meterH);
    auto rMeterBox = juce::Rectangle<float>(area.getX() + 126.0f, area.getY() + 6.0f, meterW, meterH);

    drawAnalogVUMeter(g, lMeterBox, smoothedLeftVU, "VU IN");
    drawAnalogVUMeter(g, rMeterBox, smoothedRightVU, "VU OUT");

    // Bottom digital scale display bar inside acrylic window
    auto statusBox = juce::Rectangle<float>(area.getX() + 6.0f, area.getY() + 88.0f, area.getWidth() - 12.0f, 20.0f);
    g.setColour(juce::Colour(0xff050308));
    g.fillRoundedRectangle(statusBox, 3.0f);
    g.setColour(juce::Colour(0xff2a1b3d));
    g.drawRoundedRectangle(statusBox, 3.0f, 0.8f);

    juce::String autoTuneText = juce::String("AUTO-TUNE: ") + VeviAudio::NoteNames[currentRootIndex] + " " + scaleSelector.getText();
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00f5ff));
    g.drawText(autoTuneText, statusBox.toNearestInt(), juce::Justification::centred);
}

//==============================================================================
// Dedicated Hardware Backplate for Dynamics Vertical Faders
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawDynamicsFaderPlate(juce::Graphics& g, juce::Rectangle<float> area,
                                                             const juce::StringArray& scaleMarks, bool marksOnLeft)
{
    // 1. Drop shadow
    g.setColour(juce::Colour(0x66000000));
    g.fillRoundedRectangle(area.translated(0.0f, 3.0f), 5.0f);

    // 2. Base plate: Dark Brushed Obsidian / Titanium
    juce::ColourGradient plateGrad(
        juce::Colour(0xff181228), area.getX(), area.getY(),
        juce::Colour(0xff090614), area.getRight(), area.getBottom(), false);
    plateGrad.addColour(0.30, juce::Colour(0xff231938));
    plateGrad.addColour(0.70, juce::Colour(0xff120c1e));
    g.setGradientFill(plateGrad);
    g.fillRoundedRectangle(area, 5.0f);

    // Subtle vertical brushed lines
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.035f));
    for (float xLine = area.getX() + 3.0f; xLine < area.getRight() - 3.0f; xLine += 3.0f)
        g.drawVerticalLine((int)xLine, area.getY() + 3.0f, area.getBottom() - 3.0f);

    // Metallic beveled border with subtle neon violet rim
    juce::ColourGradient borderGrad(
        juce::Colour(0xff8a2be2).withAlpha(0.70f), area.getX(), area.getY(),
        juce::Colour(0xff3b1a64), area.getRight(), area.getBottom(), false);
    g.setGradientFill(borderGrad);
    g.drawRoundedRectangle(area, 5.0f, 1.2f);

    // 3. 4 Micro Hex Screws on corners
    float screwOffset = 7.0f;
    juce::Point<float> corners[] = {
        { area.getX() + screwOffset, area.getY() + screwOffset },
        { area.getRight() - screwOffset, area.getY() + screwOffset },
        { area.getX() + screwOffset, area.getBottom() - screwOffset },
        { area.getRight() - screwOffset, area.getBottom() - screwOffset }
    };
    for (const auto& pt : corners)
    {
        g.setColour(juce::Colour(0xff0c0818));
        g.fillEllipse(pt.x - 3.0f, pt.y - 3.0f, 6.0f, 6.0f);
        g.setColour(juce::Colour(0xffa855f7).withAlpha(0.6f));
        g.fillEllipse(pt.x - 2.0f, pt.y - 2.0f, 4.0f, 4.0f);
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        g.drawLine(pt.x - 1.5f, pt.y, pt.x + 1.5f, pt.y, 0.7f);
    }

    // 4. Vertical Recessed Slot
    float slotW = 5.0f;
    float slotX = area.getCentreX() - slotW * 0.5f;
    float slotY = area.getY() + 10.0f;
    float slotH = area.getHeight() - 20.0f;

    auto slotRect = juce::Rectangle<float>(slotX, slotY, slotW, slotH);
    g.setColour(juce::Colour(0xff06030c));
    g.fillRoundedRectangle(slotRect, 2.5f);
    g.setColour(juce::Colour(0xff2d1748));
    g.drawRoundedRectangle(slotRect, 2.5f, 1.0f);

    // Center illuminated laser guide line
    g.setColour(juce::Colour(0xffa855f7).withAlpha(0.40f));
    g.drawVerticalLine((int)(slotX + slotW * 0.5f), slotY + 2.0f, slotY + slotH - 2.0f);

    // 5. Etched Calibration Markings & Scale Numbers
    int numMarks = scaleMarks.size();
    if (numMarks > 1)
    {
        g.setFont(juce::Font(8.0f, juce::Font::bold));
        for (int i = 0; i < numMarks; ++i)
        {
            float relY = (float)i / (float)(numMarks - 1);
            float my = slotY + 4.0f + relY * (slotH - 8.0f);

            // Tick lines
            g.setColour(juce::Colour(0xffc084fc).withAlpha(0.70f));
            g.drawLine(slotX - 6.0f, my, slotX - 2.0f, my, 1.0f);
            g.drawLine(slotX + slotW + 2.0f, my, slotX + slotW + 6.0f, my, 1.0f);

            // Text
            g.setColour(juce::Colour(0xffe9d5ff));
            if (marksOnLeft)
                g.drawText(scaleMarks[i], (int)(slotX - 28.0f), (int)(my - 5.0f), 22, 10, juce::Justification::right);
            else
                g.drawText(scaleMarks[i], (int)(slotX + slotW + 8.0f), (int)(my - 5.0f), 22, 10, juce::Justification::left);
        }
    }
}

//==============================================================================
// 24K Brushed Gold Master Plate (El Fader Dorado)
//==============================================================================
void TPainSupremeAudioProcessorEditor::draw24KGoldMasterPlate(juce::Graphics& g, juce::Rectangle<float> area)
{
    g.setColour(juce::Colour(0x66000000));
    g.fillRoundedRectangle(area.translated(0.0f, 3.0f), 5.0f);

    juce::ColourGradient goldPlate(
        juce::Colour(0xffd4af37), area.getX(), area.getY(),
        juce::Colour(0xff8c6214), area.getRight(), area.getBottom(), false);
    goldPlate.addColour(0.25, juce::Colour(0xfffff099));
    goldPlate.addColour(0.65, juce::Colour(0xffb8860b));
    g.setGradientFill(goldPlate);
    g.fillRoundedRectangle(area, 5.0f);

    // Subtle brushed lines
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.06f));
    for (float xLine = area.getX() + 3.0f; xLine < area.getRight() - 3.0f; xLine += 3.0f)
        g.drawVerticalLine((int)xLine, area.getY() + 3.0f, area.getBottom() - 3.0f);

    g.setColour(juce::Colour(0xffffe680));
    g.drawRoundedRectangle(area, 5.0f, 1.2f);

    // 4 Corner Gold Hex Screws
    float screwOffset = 10.0f;
    juce::Point<float> corners[] = {
        { area.getX() + screwOffset, area.getY() + screwOffset },
        { area.getRight() - screwOffset, area.getY() + screwOffset },
        { area.getX() + screwOffset, area.getBottom() - screwOffset },
        { area.getRight() - screwOffset, area.getBottom() - screwOffset }
    };

    for (const auto& pt : corners)
    {
        g.setColour(juce::Colour(0xff573a06));
        g.fillEllipse(pt.x - 4.0f, pt.y - 4.0f, 8.0f, 8.0f);
        g.setColour(juce::Colour(0xfffff2a3));
        g.fillEllipse(pt.x - 3.0f, pt.y - 3.0f, 6.0f, 6.0f);
        g.setColour(juce::Colour(0xff3d2703));
        g.drawLine(pt.x - 2.0f, pt.y, pt.x + 2.0f, pt.y, 0.8f);
    }

    // Vertical Slot
    float slotW = 5.0f;
    float slotX = area.getCentreX() - slotW * 0.5f;
    float slotY = area.getY() + 16.0f;
    float slotH = area.getHeight() - 44.0f;

    auto slotRect = juce::Rectangle<float>(slotX, slotY, slotW, slotH);
    g.setColour(juce::Colour(0xff090604));
    g.fillRoundedRectangle(slotRect, 2.5f);
    g.setColour(juce::Colour(0xff573a06));
    g.drawRoundedRectangle(slotRect, 2.5f, 1.0f);

    // Precision etched dB markings (+6 dB to -24 dB)
    struct DBMark { float relY; const char* txt; };
    DBMark marks[] = {
        { 0.06f, "+6" },
        { 0.18f, "+3" },
        { 0.32f, "0" },
        { 0.48f, "-6" },
        { 0.65f, "-12" },
        { 0.80f, "-18" },
        { 0.94f, "-24" }
    };

    g.setFont(juce::Font(8.0f, juce::Font::bold));
    for (const auto& m : marks)
    {
        float my = slotY + m.relY * slotH;
        g.setColour(juce::Colour(0xff452d06));
        g.drawLine(slotX - 7.0f, my, slotX - 2.0f, my, 1.0f);
        g.drawLine(slotX + slotW + 2.0f, my, slotX + slotW + 7.0f, my, 1.0f);
        g.drawText(m.txt, (int)(slotX - 28.0f), (int)(my - 5.0f), 20, 10, juce::Justification::right);
    }

    // Serigraphy at bottom: "3.9 MASTER LEVEL • BY VEVI"
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff4a3106));
    g.drawText("3.9 MASTER LEVEL", (int)area.getX(), (int)(area.getBottom() - 22.0f), (int)area.getWidth(), 12, juce::Justification::centred);
    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.drawText("BY VEVI", (int)area.getX(), (int)(area.getBottom() - 11.0f), (int)area.getWidth(), 10, juce::Justification::centred);
}

// Unused rack stubs
void TPainSupremeAudioProcessorEditor::drawHardwareRackEars(juce::Graphics&, int, int) {}
void TPainSupremeAudioProcessorEditor::drawSegmentedPitchWheel(juce::Graphics&, juce::Rectangle<float>) {}
void TPainSupremeAudioProcessorEditor::drawVerticalDigitalBarMeters(juce::Graphics&, juce::Rectangle<float>) {}
void TPainSupremeAudioProcessorEditor::drawQuarterInchJack(juce::Graphics&, float, float, float) {}

//==============================================================================
// Main Paint Method (v3.6 Hybrid Edition)
//==============================================================================
void TPainSupremeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // 1. Deep cosmic base gradient (dark navy)
    juce::ColourGradient bgGrad(
        juce::Colour(0xff0a0f1e), (float)getWidth() * 0.5f, 0.0f,
        juce::Colour(0xff03050c), (float)getWidth() * 0.5f, (float)getHeight(), false);
    g.setGradientFill(bgGrad);
    g.fillAll();

    // 2. High-Tech Cybernetic Texture at 20% Opacity (Hardware Texture Layer)
    if (bgTextureImage.isValid())
    {
        g.setOpacity(0.20f);
        g.drawImage(bgTextureImage, getLocalBounds().toFloat(), juce::RectanglePlacement::fillDestination);
        g.setOpacity(1.0f);
    }

    // Violet ambient orb (top-left)
    juce::ColourGradient violetOrb(
        juce::Colour(0xff5c1aaa).withAlpha(0.20f), (float)getWidth() * 0.18f, (float)getHeight() * 0.12f,
        juce::Colours::transparentBlack, (float)getWidth() * 0.65f, (float)getHeight() * 0.55f, true);
    g.setGradientFill(violetOrb);
    g.fillAll();

    // Pink ambient orb (bottom-right)
    juce::ColourGradient pinkOrb(
        juce::Colour(0xffcc0060).withAlpha(0.13f), (float)getWidth() * 0.88f, (float)getHeight() * 0.82f,
        juce::Colours::transparentBlack, (float)getWidth() * 0.42f, (float)getHeight() * 0.35f, true);
    g.setGradientFill(pinkOrb);
    g.fillAll();

    // Subtle grid
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.018f));
    for (int y = 0; y < getHeight(); y += 44)
        g.drawHorizontalLine(y, 0.0f, (float)getWidth());

    // 2. Liquid Glass Header (Y: 0-82)
    auto headerF = juce::Rectangle<float>(0.0f, 0.0f, (float)getWidth(), 82.0f);

    juce::ColourGradient hdrBase(
        juce::Colour(0xff141030).withAlpha(0.92f), 0.0f, 0.0f,
        juce::Colour(0xff090618).withAlpha(0.88f), 0.0f, 82.0f, false);
    g.setGradientFill(hdrBase);
    g.fillRect(headerF);

    juce::ColourGradient hdrLine(
        juce::Colour(0xff8a2be2), 0.0f, 81.0f,
        juce::Colour(0xffff007f), (float)getWidth(), 81.0f, false);
    g.setGradientFill(hdrLine);
    g.fillRect(0, 80, getWidth(), 2);

    g.setFont(juce::Font(26.0f, juce::Font::bold));
    juce::ColourGradient titleGrad(
        juce::Colour(0xffe6c2ff), 26.0f, 16.0f,
        juce::Colour(0xffff2a85), 260.0f, 40.0f, false);
    g.setGradientFill(titleGrad);
    g.drawText("SUPREME TUNER", 28, 16, 250, 30, juce::Justification::left);

    // Version Badge: Real Time v3.9
    auto vBadge = juce::Rectangle<float>(286.0f, 20.0f, 116.0f, 22.0f);
    g.setColour(juce::Colour(0xffff007f).withAlpha(0.18f));
    g.fillRoundedRectangle(vBadge, 6.0f);
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.22f));
    g.drawRoundedRectangle(vBadge, 6.0f, 1.0f);
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffccee));
    g.drawText("REAL TIME v3.9", vBadge.toNearestInt(), juce::Justification::centred);

    // Subtitle with Author: vevi
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffd8b4fe));
    g.drawText("BY VEVI", 28, 48, 60, 18, juce::Justification::left);
    g.setFont(juce::Font(11.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff9080b8));
    g.drawText("|  ULTRA-LOW LATENCY VOCAL QUANTIZATION ENGINE", 82, 48, 340, 18, juce::Justification::left);

    // Header selector labels
    g.setFont(juce::Font(11.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffc77dff));
    g.drawText("ARTIST PRESET", 430, 20, 130, 16, juce::Justification::left);
    g.drawText("KEY / ROOT",    690, 20, 100, 16, juce::Justification::left);
    g.drawText("SCALE TYPE",    820, 20, 130, 16, juce::Justification::left);

    // Latency badge: Liquid Glass
    auto latBadge = juce::Rectangle<float>((float)getWidth() - 240.0f, 18.0f, 214.0f, 44.0f);
    g.setColour(juce::Colour(0xff1a1035).withAlpha(0.85f));
    g.fillRoundedRectangle(latBadge, 8.0f);
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.16f));
    g.drawRoundedRectangle(latBadge, 8.0f, 1.0f);

    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffff007f));
    g.drawText("[LIVE AUDIO ENGINE]", (int)latBadge.getX() + 10, (int)latBadge.getY() + 5, 194, 14, juce::Justification::left);
    g.setFont(juce::Font(11.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xfff0eaff));
    g.drawText("LATENCY: < 2.9 ms | 64-BIT", (int)latBadge.getX() + 10, (int)latBadge.getY() + 21, 194, 18, juce::Justification::left);

    // 3. Middle Stage: Tuning Rack Left, Antares Pitch Wheel Center, Tuning Rack Right
    float midY = 96.0f;
    float midH = 328.0f;
    float sideW = 290.0f;

    auto leftTuningBox = juce::Rectangle<float>(24.0f, midY, sideW, midH);
    drawWavesRackUnit(g, leftTuningBox, "PITCH QUANTIZE", juce::Colour(0xffff007f));

    auto rightTuningBox = juce::Rectangle<float>(getWidth() - 24.0f - sideW, midY, sideW, midH);
    drawWavesRackUnit(g, rightTuningBox, "GLIDE & VIBRATO", juce::Colour(0xff8a2be2));

    // Antares Real-Time Pitch Wheel Stage
    float wheelX = 24.0f + sideW + 14.0f;
    float wheelW = (getWidth() - 24.0f - sideW) - wheelX - 14.0f;
    auto wheelArea = juce::Rectangle<float>(wheelX, midY, wheelW, midH);
    drawAntaresPitchWheel(g, wheelArea);

    // T-Pain Hard Snap Badge
    auto* rawSpeedParam = audioProcessor.apvts.getRawParameterValue(TPainSupremeAudioProcessor::ID_RETUNE_SPEED);
    float currentSpeed = (rawSpeedParam != nullptr) ? rawSpeedParam->load() : 10.0f;
    if (currentSpeed <= 0.5f)
    {
        auto tPainBadge = juce::Rectangle<int>(38, static_cast<int>(midY) + 288, static_cast<int>(sideW) - 28, 26);
        g.setColour(juce::Colour(0xffff007f).withAlpha(0.28f));
        g.fillRoundedRectangle(tPainBadge.toFloat(), 5.0f);
        g.setColour(juce::Colour(0xffff007f));
        g.drawRoundedRectangle(tPainBadge.toFloat(), 5.0f, 1.4f);
        g.setFont(juce::Font(11.5f, juce::Font::bold));
        g.setColour(juce::Colour(0xffffe6f2));
        g.drawText("[T-PAIN HARD SNAP: ACTIVE]", tPainBadge, juce::Justification::centred);
    }
    else
    {
        auto natBadge = juce::Rectangle<int>(38, static_cast<int>(midY) + 288, static_cast<int>(sideW) - 28, 26);
        g.setColour(juce::Colour(0xff22143d));
        g.fillRoundedRectangle(natBadge.toFloat(), 5.0f);
        g.setColour(juce::Colour(0xff4c3078));
        g.drawRoundedRectangle(natBadge.toFloat(), 5.0f, 1.0f);
        g.setFont(juce::Font(11.5f, juce::Font::bold));
        g.setColour(juce::Colour(0xffcbb9f7));
        g.drawText("NATURAL PITCH CORRECTION", natBadge, juce::Justification::centred);
    }

    // 4. Lower Waves Studio Rack Units (Y: 438, Height: 382)
    int rackY = 438;
    int rackH = 382;

    // Unit 1: NOISE GATE & DYNAMICS (X: 24, W: 290)
    auto dynBox = juce::Rectangle<float>(24.0f, static_cast<float>(rackY), 290.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, dynBox, "NOISE GATE & DYNAMICS", juce::Colour(0xffff007f));

    // Gate Status Badge & LED (Y: rackY + 120 = 558 to 580)
    auto gateBadgeArea = juce::Rectangle<int>(38, rackY + 120, 262, 22);
    g.setColour(juce::Colour(0xff120924));
    g.fillRoundedRectangle(gateBadgeArea.toFloat(), 4.0f);
    g.setColour(isGateOpen ? juce::Colour(0xff00f5ff).withAlpha(0.35f) : juce::Colour(0xffa855f7).withAlpha(0.35f));
    g.drawRoundedRectangle(gateBadgeArea.toFloat(), 4.0f, 1.0f);

    float ledX = static_cast<float>(gateBadgeArea.getX() + 12);
    float ledY = static_cast<float>(gateBadgeArea.getCentreY());
    juce::Colour ledColor = isGateOpen ? juce::Colour(0xff00f5ff) : juce::Colour(0xffa855f7);
    g.setColour(ledColor);
    g.fillEllipse(ledX - 3.5f, ledY - 3.5f, 7.0f, 7.0f);
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.fillEllipse(ledX - 1.5f, ledY - 1.5f, 3.0f, 3.0f);

    g.setFont(juce::Font(10.0f, juce::Font::bold));
    if (isGateOpen)
    {
        g.setColour(juce::Colour(0xffe0ffff));
        g.drawText("GATE: OPEN (VOICE ACTIVE)", gateBadgeArea.getX() + 24, gateBadgeArea.getY(), gateBadgeArea.getWidth() - 28, gateBadgeArea.getHeight(), juce::Justification::centredLeft);
    }
    else
    {
        g.setColour(juce::Colour(0xffe9d5ff));
        g.drawText("GATE: ATTENUATING -" + juce::String(currentGateGR, 1) + " dB", gateBadgeArea.getX() + 24, gateBadgeArea.getY(), gateBadgeArea.getWidth() - 28, gateBadgeArea.getHeight(), juce::Justification::centredLeft);
    }

    // Dedicated Hardware Backplates for TRANSIENTS and COMPRESSION Faders
    auto transPlate = juce::Rectangle<float>(48.0f, static_cast<float>(rackY + 164), 78.0f, 102.0f);
    drawDynamicsFaderPlate(g, transPlate, { "+12", "+6", "0", "-6", "-12" }, true);

    auto compPlate = juce::Rectangle<float>(188.0f, static_cast<float>(rackY + 164), 78.0f, 102.0f);
    drawDynamicsFaderPlate(g, compPlate, { "100", "75", "50", "25", "0" }, false);

    // Dynamic Compression Gain Reduction VU Meter
    auto grMeterArea = juce::Rectangle<int>(42, rackY + 296, 254, 16);
    g.setColour(juce::Colour(0xff130b22));
    g.fillRoundedRectangle(grMeterArea.toFloat(), 4.0f);
    g.setColour(juce::Colour(0xff472b73));
    g.drawRoundedRectangle(grMeterArea.toFloat(), 4.0f, 1.0f);

    float grNorm = juce::jlimit(0.0f, 1.0f, currentGR / 18.0f);
    if (grNorm > 0.01f)
    {
        auto activeGR = grMeterArea.removeFromLeft(static_cast<int>(grMeterArea.getWidth() * grNorm));
        juce::ColourGradient grGrad(juce::Colour(0xffff007f), static_cast<float>(activeGR.getX()), static_cast<float>(activeGR.getY()),
                                   juce::Colour(0xffff55aa), static_cast<float>(activeGR.getRight()), static_cast<float>(activeGR.getY()), false);
        g.setGradientFill(grGrad);
        g.fillRoundedRectangle(activeGR.toFloat(), 3.0f);
    }
    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffe6c2ff));
    g.drawText("COMP GAIN REDUCTION", 42, rackY + 316, 140, 16, juce::Justification::left);
    g.drawText("--" + juce::String(currentGR, 1) + " dB", 196, rackY + 316, 100, 16, juce::Justification::right);

    // Unit 2: VOCAL TONE (X: 328, W: 290)
    auto toneBox = juce::Rectangle<float>(328.0f, static_cast<float>(rackY), 290.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, toneBox, "VOCAL TONE ENGINE", juce::Colour(0xff8a2be2));

    // Unit 3: SPACE FX (X: 632, W: 310)
    auto spaceBox = juce::Rectangle<float>(632.0f, static_cast<float>(rackY), 310.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, spaceBox, "SPACE & AMBIENCE", juce::Colour(0xffff00aa));

    // Unit 4: GAIN STAGING & MASTER (X: 956, W: 270)
    auto masterBox = juce::Rectangle<float>(956.0f, static_cast<float>(rackY), 270.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, masterBox, "GAIN STAGING & MASTER", juce::Colour(0xff8a2be2));

    // TOP OF UNIT 4: DUAL VINTAGE ANALOG VU METERS (Búmetros)
    auto vuBayArea = juce::Rectangle<float>(968.0f, static_cast<float>(rackY + 38), 246.0f, 114.0f);
    drawDualAnalogVUMeters(g, vuBayArea);

    // RIGHT SIDE OF UNIT 4: 24K BRUSHED GOLD MASTER FADER PLATE
    auto goldBayArea = juce::Rectangle<float>(1074.0f, static_cast<float>(rackY + 160), 136.0f, 172.0f);
    draw24KGoldMasterPlate(g, goldBayArea);

    // Analog Tape Saturation Active Badge at bottom
    auto satBadge = juce::Rectangle<int>(972, rackY + 342, 238, 24);
    g.setColour(juce::Colour(0xffff007f).withAlpha(0.20f));
    g.fillRoundedRectangle(satBadge.toFloat(), 5.0f);
    g.setColour(juce::Colour(0xffff007f));
    g.drawRoundedRectangle(satBadge.toFloat(), 5.0f, 1.2f);
    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffb8dd));
    g.drawText("ANALOG SATURATION: ACTIVE", satBadge, juce::Justification::centred);
}

//==============================================================================
// Liquid Glass Waves Rack Unit
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawWavesRackUnit(juce::Graphics& g, juce::Rectangle<float> bounds,
                                                       const juce::String& title, const juce::Colour& headerGlow)
{
    float cr = 16.0f;

    auto shadow = bounds.translated(0.0f, 4.0f).expanded(3.0f, 1.5f);
    juce::ColourGradient shadowGrad(
        juce::Colour(0xff000000).withAlpha(0.45f), shadow.getCentreX(), shadow.getY(),
        juce::Colours::transparentBlack, shadow.getCentreX(), shadow.getBottom(), false);
    g.setGradientFill(shadowGrad);
    g.fillRoundedRectangle(shadow, cr + 2.0f);

    juce::ColourGradient glassBase(
        juce::Colour(0xff100c22).withAlpha(0.82f), bounds.getX(), bounds.getY(),
        juce::Colour(0xff070412).withAlpha(0.75f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(glassBase);
    g.fillRoundedRectangle(bounds, cr);

    juce::ColourGradient tint(
        headerGlow.withAlpha(0.10f), bounds.getX(), bounds.getY(),
        juce::Colours::transparentBlack, bounds.getX(), bounds.getY() + bounds.getHeight() * 0.45f, false);
    g.setGradientFill(tint);
    g.fillRoundedRectangle(bounds, cr);

    float sheenH = bounds.getHeight() * 0.40f;
    juce::ColourGradient sheen(
        juce::Colour(0xffffffff).withAlpha(0.07f), bounds.getCentreX(), bounds.getY(),
        juce::Colours::transparentWhite, bounds.getCentreX(), bounds.getY() + sheenH, false);
    g.setGradientFill(sheen);
    g.fillRoundedRectangle(bounds.withHeight(sheenH), cr);

    g.setColour(juce::Colour(0xffffffff).withAlpha(0.15f));
    g.drawRoundedRectangle(bounds, cr, 1.0f);

    auto titleBar = bounds.removeFromTop(34.0f);
    juce::ColourGradient titleBg(
        headerGlow.withAlpha(0.22f), titleBar.getX(), titleBar.getY(),
        headerGlow.withAlpha(0.06f), titleBar.getX(), titleBar.getBottom(), false);
    g.setGradientFill(titleBg);
    g.fillRoundedRectangle(titleBar, cr);

    g.setColour(headerGlow.withAlpha(0.55f));
    g.fillRect(titleBar.getX() + 16.0f, titleBar.getBottom() - 1.5f, titleBar.getWidth() - 32.0f, 1.5f);

    g.setFont(juce::Font(12.5f, juce::Font::bold));
    g.setColour(headerGlow.interpolatedWith(juce::Colours::white, 0.65f));
    g.drawText(title, titleBar.reduced(16.0f, 0.0f), juce::Justification::centredLeft);
}

//==============================================================================
// Antares Liquid Glass Real-Time Chromatic Pitch Wheel
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawAntaresPitchWheel(juce::Graphics& g, juce::Rectangle<float> area)
{
    float cr = 16.0f;

    auto wShadow = area.translated(0.0f, 4.0f).expanded(3.0f, 1.5f);
    g.setColour(juce::Colour(0xff000000).withAlpha(0.42f));
    g.fillRoundedRectangle(wShadow, cr + 2.0f);

    juce::ColourGradient wBase(
        juce::Colour(0xff0e0a22).withAlpha(0.86f), area.getX(), area.getY(),
        juce::Colour(0xff060310).withAlpha(0.80f), area.getX(), area.getBottom(), false);
    g.setGradientFill(wBase);
    g.fillRoundedRectangle(area, cr);

    g.setColour(juce::Colour(0xffffffff).withAlpha(0.14f));
    g.drawRoundedRectangle(area, cr, 1.0f);

    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffff007f).withAlpha(0.80f));
    g.drawText("REAL-TIME CHROMATIC PITCH WHEEL  •  BY VEVI", (int)area.getX() + 18, (int)area.getY() + 10, 360, 20, juce::Justification::left);

    float cx = area.getCentreX();
    float cy = area.getY() + 144.0f;
    float outerRadius = 104.0f;
    float innerRadius = 56.0f;

    g.setColour(juce::Colour(0xff22143e));
    g.drawEllipse(cx - outerRadius, cy - outerRadius, outerRadius * 2.0f, outerRadius * 2.0f, 1.8f);
    g.drawEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.2f);

    VeviAudio::ScaleType scaleType = static_cast<VeviAudio::ScaleType>(currentScaleIndex);
    uint16_t scaleMask = VeviAudio::PitchEngine::getScaleMask(scaleType);

    int activeDetectedIndex = -1;
    if (isVoiced && currentHz > 50.0f)
    {
        int detectedMidi = audioProcessor.getDetectedMidiNote();
        activeDetectedIndex = (detectedMidi % 12 + 12) % 12;
    }

    for (int i = 0; i < 12; ++i)
    {
        float angle = (static_cast<float>(i) / 12.0f) * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
        float nx = cx + outerRadius * std::cos(angle);
        float ny = cy + outerRadius * std::sin(angle);

        int semitoneRelRoot = ((i - currentRootIndex) % 12 + 12) % 12;
        bool inScale = (scaleMask & (1 << semitoneRelRoot)) != 0;
        bool isDetected = (activeDetectedIndex == i);

        float nodeW = isDetected ? 34.0f : 28.0f;
        auto nodeRect = juce::Rectangle<float>(nx - nodeW * 0.5f, ny - nodeW * 0.5f, nodeW, nodeW);

        if (isDetected)
        {
            g.setColour(juce::Colour(0xffff007f).withAlpha(0.35f));
            g.fillEllipse(nx - 28.0f, ny - 28.0f, 56.0f, 56.0f);
            g.setColour(juce::Colour(0xffff007f));
            g.fillEllipse(nodeRect);
            g.setColour(juce::Colours::white);
            g.drawEllipse(nodeRect, 2.0f);
            g.setFont(juce::Font(12.0f, juce::Font::bold));
            g.setColour(juce::Colours::white);
            g.drawText(VeviAudio::NoteNames[i], nodeRect, juce::Justification::centred);
        }
        else if (inScale)
        {
            g.setColour(juce::Colour(0xff2a164d));
            g.fillEllipse(nodeRect);
            g.setColour(juce::Colour(0xffa855f7));
            g.drawEllipse(nodeRect, 1.4f);
            g.setFont(juce::Font(10.5f, juce::Font::bold));
            g.setColour(juce::Colour(0xfff3eaff));
            g.drawText(VeviAudio::NoteNames[i], nodeRect, juce::Justification::centred);
        }
        else
        {
            g.setColour(juce::Colour(0xff140c24));
            g.fillEllipse(nodeRect);
            g.setColour(juce::Colour(0xff33214e));
            g.drawEllipse(nodeRect, 1.0f);
            g.setFont(juce::Font(9.5f, juce::Font::plain));
            g.setColour(juce::Colour(0xff675482));
            g.drawText(VeviAudio::NoteNames[i], nodeRect, juce::Justification::centred);
        }
    }

    // Laser needle
    if (isVoiced && currentHz > 50.0f)
    {
        float needleRadius = outerRadius - 16.0f;
        float tipX = cx + needleRadius * std::cos(smoothedPitchAngle);
        float tipY = cy + needleRadius * std::sin(smoothedPitchAngle);

        juce::Path needle;
        needle.startNewSubPath(cx, cy);
        needle.lineTo(tipX, tipY);

        g.setColour(juce::Colour(0xffff007f).withAlpha(0.45f));
        g.strokePath(needle, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour(juce::Colour(0xffffffff));
        g.strokePath(needle, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Central Liquid Glass Orb
    auto hubRect = juce::Rectangle<float>(cx - innerRadius + 4.0f, cy - innerRadius + 4.0f,
                                          (innerRadius - 4.0f) * 2.0f, (innerRadius - 4.0f) * 2.0f);
    juce::ColourGradient hubBase(
        juce::Colour(0xff0e0820).withAlpha(0.90f), hubRect.getX() + hubRect.getWidth() * 0.35f, hubRect.getY() + hubRect.getHeight() * 0.25f,
        juce::Colour(0xff060412).withAlpha(0.95f), hubRect.getRight(), hubRect.getBottom(), true);
    g.setGradientFill(hubBase);
    g.fillEllipse(hubRect);
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.20f));
    g.drawEllipse(hubRect, 1.2f);

    if (isVoiced && currentHz > 50.0f)
    {
        g.setFont(juce::Font(26.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xffff007f));
        g.drawText(currentTargetNote, (int)hubRect.getX(), (int)hubRect.getY() + 12, (int)hubRect.getWidth(), 28, juce::Justification::centred);

        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xffe6c2ff));
        g.drawText(juce::String(currentHz, 1) + " Hz", (int)hubRect.getX(), (int)hubRect.getY() + 42, (int)hubRect.getWidth(), 16, juce::Justification::centred);

        juce::String centsStr = (currentCentsDeviation >= 0.0f ? "+" : "") + juce::String(currentCentsDeviation, 1) + "c";
        g.setFont(juce::Font(11.5f, juce::Font::bold));
        g.setColour(juce::Colour(0xffff75bc));
        g.drawText(centsStr, (int)hubRect.getX(), (int)hubRect.getY() + 60, (int)hubRect.getWidth(), 16, juce::Justification::centred);
    }
    else
    {
        g.setFont(juce::Font(24.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xff573a7f));
        g.drawText("--", (int)hubRect.getX(), (int)hubRect.getY() + 22, (int)hubRect.getWidth(), 28, juce::Justification::centred);

        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xff7d5ca6));
        g.drawText("READY", hubRect.toNearestInt(), juce::Justification::centred);
    }

    // Cents Meter Scale Under Wheel
    float meterY = area.getY() + 292.0f;
    auto meterArea = juce::Rectangle<float>(cx - 120.0f, meterY, 240.0f, 7.0f);
    g.setColour(juce::Colour(0xff180e2d));
    g.fillRoundedRectangle(meterArea, 3.5f);
    g.setColour(juce::Colour(0xff43266f));
    g.drawRoundedRectangle(meterArea, 3.5f, 1.0f);

    g.setColour(juce::Colour(0xffa855f7));
    g.fillRect(cx - 1.0f, meterArea.getY() - 3.0f, 2.0f, 13.0f);

    if (isVoiced)
    {
        float clampedCents = juce::jlimit(-50.0f, 50.0f, currentCentsDeviation);
        float normCents = (clampedCents + 50.0f) / 100.0f;
        float barX = meterArea.getX() + normCents * meterArea.getWidth();

        auto bar = juce::Rectangle<float>(std::min(cx, barX), meterArea.getY(), std::abs(barX - cx), meterArea.getHeight());
        g.setColour(juce::Colour(0xffff007f));
        g.fillRoundedRectangle(bar, 2.5f);
    }

    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff947dae));
    g.drawText("-50c", (int)(meterArea.getX() - 34.0f), (int)(meterArea.getY() - 3.0f), 30, 14, juce::Justification::right);
    g.drawText("0c",   (int)(cx - 12.0f), (int)(meterArea.getBottom() + 3.0f), 24, 14, juce::Justification::centred);
    g.drawText("+50c", (int)(meterArea.getRight() + 4.0f), (int)(meterArea.getY() - 3.0f), 34, 14, juce::Justification::left);
}

//==============================================================================
// Resized Layout: Perfect Zero-Overlap Modular Studio Layout
//==============================================================================
void TPainSupremeAudioProcessorEditor::resized()
{
    // Top Selectors
    presetSelector.setBounds(430, 42, 240, 28);
    rootSelector.setBounds(690, 42, 115, 28);
    scaleSelector.setBounds(820, 42, 145, 28);

    // Middle Stage Y: 96, Height: 328
    retuneSpeedSlider.setBounds(38, 130, 130, 134);
    speedLabel.setBounds(30, 268, 146, 16);

    transitionSlider.setBounds(184, 142, 108, 122);
    transSpeedLabel.setBounds(176, 268, 124, 16);

    centeringSlider.setBounds(getWidth() - 274, 142, 108, 122);
    centerLabel.setBounds(getWidth() - 282, 268, 124, 16);

    variationSlider.setBounds(getWidth() - 146, 142, 108, 122);
    varLabel.setBounds(getWidth() - 154, 268, 124, 16);

    // Lower Rack Units (Y: 438, Height: 382)
    int rackY = 438;

    // Unit 1: NOISE GATE & DYNAMICS (X: 24, W: 290)
    int gateKnobY = rackY + 38;
    int gateKnobW = 76;
    int gateKnobH = 58;
    gateThreshSlider.setBounds(33, gateKnobY, gateKnobW, gateKnobH);
    gateThrLabel.setBounds(27, gateKnobY + gateKnobH + 2, 88, 14);

    gateAttackSlider.setBounds(121, gateKnobY, gateKnobW, gateKnobH);
    gateAtkLabel.setBounds(115, gateKnobY + gateKnobH + 2, 88, 14);

    gateReleaseSlider.setBounds(209, gateKnobY, gateKnobW, gateKnobH);
    gateRelLabel.setBounds(203, gateKnobY + gateKnobH + 2, 88, 14);

    transLabel.setBounds(42, rackY + 148, 90, 14);
    compLabel.setBounds(182, rackY + 148, 90, 14);

    transientsSlider.setBounds(52, rackY + 164, 70, 126);
    compressionSlider.setBounds(192, rackY + 164, 70, 126);

    // Unit 2: VOCAL TONE (X: 328, W: 290)
    int toneKnobY = rackY + 110;
    int toneKnobW = 84;
    int toneKnobH = 88;
    lowCutSlider.setBounds(336, toneKnobY, toneKnobW, toneKnobH);
    lowCutLabel.setBounds(328, toneKnobY + toneKnobH + 4, 100, 16);

    bodySlider.setBounds(431, toneKnobY, toneKnobW, toneKnobH);
    bodyLabel.setBounds(423, toneKnobY + toneKnobH + 4, 100, 16);

    airSlider.setBounds(526, toneKnobY, toneKnobW, toneKnobH);
    airLabel.setBounds(518, toneKnobY + toneKnobH + 4, 100, 16);

    // Unit 3: SPACE FX (X: 632, W: 310)
    int r1Y = rackY + 46;
    int r1H = 80;
    reverbMixSlider.setBounds(672, r1Y, 82, r1H);
    revMixLabel.setBounds(662, r1Y + r1H + 2, 102, 14);

    reverbSizeSlider.setBounds(788, r1Y, 82, r1H);
    revSizeLabel.setBounds(778, r1Y + r1H + 2, 102, 14);

    int r2Y = rackY + 160;
    int r2H = 80;
    delayMixSlider.setBounds(642, r2Y, 82, r2H);
    dlyMixLabel.setBounds(632, r2Y + r2H + 2, 102, 14);

    delayTimeSlider.setBounds(746, r2Y, 82, r2H);
    dlyTimeLabel.setBounds(736, r2Y + r2H + 2, 102, 14);

    delayFeedbackSlider.setBounds(850, r2Y, 82, r2H);
    dlyFbLabel.setBounds(840, r2Y + r2H + 2, 102, 14);

    // Unit 4: GAIN STAGING & MASTER (X: 956, W: 270)
    // Left side: STEREO WIDTH & DRY / WET Knobs
    stereoWidthSlider.setBounds(974, rackY + 166, 78, 70);
    stereoWidthLabel.setBounds(968, rackY + 238, 90, 14);

    mixSlider.setBounds(974, rackY + 254, 78, 70);
    mixLabel.setBounds(968, rackY + 326, 90, 14);

    // Right side: The 24K Brushed Gold Master Fader
    outputSlider.setBounds(1074, rackY + 166, 136, 150);
    outLabel.setBounds(0, 0, 0, 0); // serigraphed on plate

    // Hidden secondary controls
    inputGainSlider.setBounds(0, 0, 0, 0);
    inputGainLabel.setBounds(0, 0, 0, 0);
}
