#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Supreme Waves Look & Feel: Luxury Studio Dark + Neon Violet & Hot Pink
//==============================================================================

SupremeWavesLookAndFeel::SupremeWavesLookAndFeel()
{
    // Liquid Glass popup menus
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

    // 1. Ambient liquid purple glow halo
    float haloR = radius + (isHero ? 14.0f : 8.0f);
    juce::Colour haloColor = isHero ? juce::Colour(0xffa855f7) : juce::Colour(0xff8a2be2);
    juce::ColourGradient halo(
        haloColor.withAlpha(isHero ? 0.30f : 0.16f),
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
    g.setColour(juce::Colour(0xff120924));
    g.strokePath(bgArc, juce::PathStrokeType(trackWidth + 1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour(juce::Colour(0xff090414));
    g.strokePath(bgArc, juce::PathStrokeType(trackWidth - 1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Active Vibrant Neon Purple / Violet Value Arc
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, arcR, arcR, 0.0f, rotaryStartAngle, currentAngle, true);

    juce::Colour violetColor = isHero ? juce::Colour(0xffa855f7) : juce::Colour(0xff9d4edd);
    juce::Colour violetBright = isHero ? juce::Colour(0xffe879f9) : juce::Colour(0xffc77dff);

    // Soft neon glow pass
    g.setColour(violetBright.withAlpha(0.40f));
    g.strokePath(valueArc, juce::PathStrokeType(trackWidth + 2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Sharp neon gradient arc
    juce::ColourGradient arcGrad(violetBright, centre.x - radius, centre.y + radius,
                                 violetColor, centre.x + radius, centre.y - radius, false);
    g.setGradientFill(arcGrad);
    g.strokePath(valueArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 4. Dark Obsidian-Violet Knob Cap Disc
    float capRadius = arcR - trackWidth * 0.5f - (isHero ? 4.5f : 3.0f);
    if (capRadius > 2.0f)
    {
        auto capX = centre.x - capRadius;
        auto capY = centre.y - capRadius;
        auto capD = capRadius * 2.0f;

        // Base gradient: dark twilight violet to obsidian
        juce::ColourGradient capBase(
            juce::Colour(0xff25173c), centre.x - capRadius * 0.45f, centre.y - capRadius * 0.55f,
            juce::Colour(0xff0d0617), centre.x + capRadius * 0.55f, centre.y + capRadius * 0.65f, true);
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

        // Outer beveled rim: metallic violet highlight
        juce::ColourGradient rimGrad(
            juce::Colour(0xff9333ea), centre.x - capRadius, centre.y - capRadius,
            juce::Colour(0xff2a1347), centre.x + capRadius, centre.y + capRadius, false);
        g.setGradientFill(rimGrad);
        g.drawEllipse(capX, capY, capD, capD, 1.2f);

        // Inner ridge
        g.setColour(juce::Colour(0xff3b1d66).withAlpha(0.7f));
        g.drawEllipse(capX + 1.2f, capY + 1.2f, capD - 2.4f, capD - 2.4f, 0.8f);

        // 5. Glowing Neon Violet Pointer Needle
        float r1 = capRadius * 0.32f;
        float r2 = capRadius * 0.86f;
        float sinA = std::sin(currentAngle);
        float cosA = -std::cos(currentAngle); // 0 at 12 o'clock

        juce::Line<float> needle(centre.x + r1 * sinA, centre.y + r1 * cosA,
                                centre.x + r2 * sinA, centre.y + r2 * cosA);

        // Neon glow pass
        g.setColour(violetBright.withAlpha(0.65f));
        g.drawLine(needle, isHero ? 4.0f : 3.2f);

        // Sharp core needle
        g.setColour(juce::Colours::white);
        g.drawLine(needle, isHero ? 2.0f : 1.6f);

        // Center jewel orb
        g.setColour(violetColor);
        g.fillEllipse(centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.fillEllipse(centre.x - 1.0f, centre.y - 1.0f, 2.0f, 2.0f);
    }
}

// THE GOLDEN MASTER FADER (Console 24K Gold Finish)
void SupremeWavesLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float minSliderPos, float maxSliderPos,
                                              const juce::Slider::SliderStyle /*style*/, juce::Slider& /*slider*/)
{
    float cx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
    float trackTop = std::min(minSliderPos, maxSliderPos);
    float trackBottom = std::max(minSliderPos, maxSliderPos);
    float trackH = trackBottom - trackTop;

    if (trackH < 10.0f)
        trackH = static_cast<float>(height) - 24.0f;

    // 1. Deep Recessed Slot with Burnished Bronze/Gold Bevel
    float slotW = 8.0f;
    auto slotRect = juce::Rectangle<float>(cx - slotW * 0.5f, trackTop, slotW, trackH);

    // Slot interior shadow
    g.setColour(juce::Colour(0xff0d0904));
    g.fillRoundedRectangle(slotRect, 4.0f);

    // Slot burnished gold rim
    g.setColour(juce::Colour(0xff6e511b));
    g.drawRoundedRectangle(slotRect, 4.0f, 1.2f);

    // Scale tick marks along sides (Pure Gold accents)
    float midY = trackTop + trackH * 0.5f;

    // Center 0 dB Tick (Bright Gold)
    g.setColour(juce::Colour(0xffffd700));
    g.fillRect(cx - 14.0f, midY - 1.0f, 28.0f, 2.0f);

    // Intermediate scale tick marks
    g.setColour(juce::Colour(0xffb8861e));
    g.fillRect(cx - 10.0f, trackTop + trackH * 0.15f - 0.5f, 20.0f, 1.2f);
    g.fillRect(cx - 10.0f, trackTop + trackH * 0.32f - 0.5f, 20.0f, 1.2f);
    g.fillRect(cx - 10.0f, trackTop + trackH * 0.68f - 0.5f, 20.0f, 1.2f);
    g.fillRect(cx - 10.0f, trackTop + trackH * 0.85f - 0.5f, 20.0f, 1.2f);
    g.fillRect(cx - 7.0f, trackTop + 2.0f, 14.0f, 1.0f);
    g.fillRect(cx - 7.0f, trackBottom - 2.0f, 14.0f, 1.0f);

    // 2. Hardware Console 24K Brushed Gold Fader Cap (Thumb)
    float thumbW = juce::jmin(38.0f, static_cast<float>(width) - 4.0f);
    float thumbH = 24.0f;
    float thumbY = sliderPos - thumbH * 0.5f;
    auto thumbRect = juce::Rectangle<float>(cx - thumbW * 0.5f, thumbY, thumbW, thumbH);

    // Warm golden drop shadow under fader cap
    g.setColour(juce::Colour(0x88000000));
    g.fillRoundedRectangle(thumbRect.translated(0.0f, 3.0f), 3.0f);
    g.setColour(juce::Colour(0xffffd700).withAlpha(0.18f));
    g.fillRoundedRectangle(thumbRect.expanded(2.0f, 1.0f), 4.0f);

    // Cap body: 24K Rich Brushed Gold gradient
    juce::ColourGradient goldCap(
        juce::Colour(0xfffff099), thumbRect.getX(), thumbRect.getY(),
        juce::Colour(0xff614407), thumbRect.getX(), thumbRect.getBottom(), false);
    goldCap.addColour(0.30, juce::Colour(0xffffd700));
    goldCap.addColour(0.65, juce::Colour(0xffb8861e));
    g.setGradientFill(goldCap);
    g.fillRoundedRectangle(thumbRect, 3.0f);

    // Top highlight bevel & bottom shadow bevel
    g.setColour(juce::Colour(0xfffffbd1));
    g.drawHorizontalLine(static_cast<int>(thumbRect.getY()), thumbRect.getX() + 1.0f, thumbRect.getRight() - 1.0f);
    g.setColour(juce::Colour(0xff3b2705));
    g.drawHorizontalLine(static_cast<int>(thumbRect.getBottom() - 1.0f), thumbRect.getX() + 1.0f, thumbRect.getRight() - 1.0f);
    g.setColour(juce::Colour(0xffd4af37));
    g.drawRoundedRectangle(thumbRect, 3.0f, 1.0f);

    // Tactile knurled finger grip ridges (dark bronze)
    g.setColour(juce::Colour(0xff4a350a));
    g.fillRect(thumbRect.getX() + 4.0f, thumbY + 5.0f, thumbRect.getWidth() - 8.0f, 1.5f);
    g.fillRect(thumbRect.getX() + 4.0f, thumbY + 17.0f, thumbRect.getWidth() - 8.0f, 1.5f);
    g.setColour(juce::Colour(0xffffe885));
    g.fillRect(thumbRect.getX() + 4.0f, thumbY + 6.5f, thumbRect.getWidth() - 8.0f, 0.7f);
    g.fillRect(thumbRect.getX() + 4.0f, thumbY + 18.5f, thumbRect.getWidth() - 8.0f, 0.7f);

    // Glowing Horizontal Center Notch Line (Bright White Core with Golden Halo)
    g.setColour(juce::Colour(0xffffd700).withAlpha(0.60f));
    g.fillRect(thumbRect.getX() + 1.0f, thumbY + 10.0f, thumbRect.getWidth() - 2.0f, 4.0f);

    g.setColour(juce::Colour(0xfffff4cc));
    g.fillRect(thumbRect.getX() + 3.0f, thumbY + 11.0f, thumbRect.getWidth() - 6.0f, 2.0f);

    g.setColour(juce::Colours::white);
    g.fillRect(thumbRect.getX() + 6.0f, thumbY + 11.5f, thumbRect.getWidth() - 12.0f, 1.0f);
}

void SupremeWavesLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                          int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                          juce::ComboBox& box)
{
    bool focused = box.hasKeyboardFocus(false) || isButtonDown;
    auto area = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f);
    float cr = 8.0f;

    // Glass base
    juce::ColourGradient base(
        juce::Colour(0xff160d2c).withAlpha(0.88f), 0.0f, 0.0f,
        juce::Colour(0xff0a0618).withAlpha(0.82f), 0.0f, (float)height, false);
    g.setGradientFill(base);
    g.fillRoundedRectangle(area, cr);

    // Glass sheen top strip
    juce::ColourGradient sheen(
        juce::Colour(0xffffffff).withAlpha(0.09f), 0.0f, 0.0f,
        juce::Colours::transparentWhite, 0.0f, height * 0.45f, false);
    g.setGradientFill(sheen);
    g.fillRoundedRectangle(area.withHeight(height * 0.45f), cr);

    // Glass border
    g.setColour(focused ? juce::Colour(0xffff007f).withAlpha(0.80f) : juce::Colour(0xffffffff).withAlpha(0.18f));
    g.drawRoundedRectangle(area, cr, 1.1f);

    // Inner border highlight
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.07f));
    g.drawRoundedRectangle(area.reduced(1.2f), cr - 1.0f, 0.7f);

    // Chevron
    juce::Path arrow;
    float ax = width - 18.0f, ay = height * 0.5f - 2.5f;
    arrow.startNewSubPath(ax, ay);
    arrow.lineTo(ax + 5.0f, ay + 5.0f);
    arrow.lineTo(ax + 10.0f, ay);
    g.setColour(focused ? juce::Colour(0xffff007f) : juce::Colour(0xffc77dff));
    g.strokePath(arrow, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

//==============================================================================
// TPainSupremeAudioProcessorEditor Implementation
//==============================================================================

TPainSupremeAudioProcessorEditor::TPainSupremeAudioProcessorEditor(TPainSupremeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&wavesLookAndFeel);

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
    retuneSpeedSlider.setName("RetuneSpeed"); // Hero knob flag

    configureKnob(transitionSlider, transSpeedLabel, "TRANSITION", " ms");
    configureKnob(variationSlider, varLabel, "VARIATION", " %");
    configureKnob(centeringSlider, centerLabel, "CENTERING", " %");

    // Configure Dynamics: ROTARY PURPLE KNOBS
    configureKnob(transientsSlider, transLabel, "TRANSIENTS", " %");
    configureKnob(compressionSlider, compLabel, "COMPRESSION", " %");

    // Configure Vocal Tone Knobs (Rotary Purple)
    configureKnob(lowCutSlider, lowCutLabel, "LOW CUT", " Hz");
    configureKnob(bodySlider, bodyLabel, "BODY", " dB");
    configureKnob(airSlider, airLabel, "AIR SHEEN", " dB");

    // Configure Space FX Knobs (Rotary Purple)
    configureKnob(reverbMixSlider, revMixLabel, "REV MIX", " %");
    configureKnob(reverbSizeSlider, revSizeLabel, "REV SIZE", " %");
    configureKnob(delayMixSlider, dlyMixLabel, "DLY MIX", " %");
    configureKnob(delayTimeSlider, dlyTimeLabel, "DLY TIME", " ms");
    configureKnob(delayFeedbackSlider, dlyFbLabel, "FEEDBACK", " %");

    // Configure Gain Staging & Master:
    configureKnob(inputGainSlider, inputGainLabel, "INPUT GAIN", " dB");
    configureKnob(stereoWidthSlider, stereoWidthLabel, "STEREO WIDTH", " %");
    configureKnob(mixSlider, mixLabel, "DRY / WET", " %");
    // MASTER VOLUME: THE ONLY FADER, PURE GOLD!
    configureGoldenMasterFader(outputSlider, outLabel, "MASTER VOL", " dB");

    // Configure Noise Gate Knobs (Rotary Purple)
    configureKnob(gateThreshSlider, gateThrLabel, "UMBRAL", " dB");
    configureKnob(gateAttackSlider, gateAtkLabel, "ATAQUE", " ms");
    configureKnob(gateReleaseSlider, gateRelLabel, "RELEASE", " ms");

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

    // Window size
    setSize(1250, 840);
    startTimerHz(30);
}

TPainSupremeAudioProcessorEditor::~TPainSupremeAudioProcessorEditor()
{
    stopTimer();

    // Explicitly reset all parameter attachments before tearing down components
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

void TPainSupremeAudioProcessorEditor::configureGoldenMasterFader(juce::Slider& slider, juce::Label& label,
                                                                 const juce::String& text, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffffd700)); // Pure Gold
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(11.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffffd700)); // Pure Gold
    addAndMakeVisible(label);
}

void TPainSupremeAudioProcessorEditor::drawLivePeakMeter(juce::Graphics& g, juce::Rectangle<float> meterArea,
                                                       float levelDb, const juce::String& /*title*/)
{
    // Background groove with subtle gold accent
    g.setColour(juce::Colour(0xff0d0914));
    g.fillRoundedRectangle(meterArea, 3.0f);
    g.setColour(juce::Colour(0xff6e511b)); // Burnished gold border
    g.drawRoundedRectangle(meterArea, 3.0f, 1.0f);

    // Map -60 dB to +6 dB (66 dB range)
    float clampedDb = juce::jlimit(-60.0f, 6.0f, levelDb);
    float norm = (clampedDb + 60.0f) / 66.0f; // 0.0 to 1.0
    float activeH = norm * (meterArea.getHeight() - 4.0f);

    if (activeH > 1.0f)
    {
        auto activeRect = juce::Rectangle<float>(meterArea.getX() + 2.0f,
                                                 meterArea.getBottom() - 2.0f - activeH,
                                                 meterArea.getWidth() - 4.0f,
                                                 activeH);

        // LED meter gradient: Green (-60 to -12 dB), Gold (-12 to 0 dB), Red (> 0 dB)
        juce::ColourGradient meterGrad(
            juce::Colour(0xff00e676), activeRect.getX(), meterArea.getBottom(),
            (clampedDb > 0.0f ? juce::Colour(0xffff1744) : juce::Colour(0xffffd700)), activeRect.getX(), meterArea.getY(), false);
        meterGrad.addColour(0.72, juce::Colour(0xffffd700));

        g.setGradientFill(meterGrad);
        g.fillRoundedRectangle(activeRect, 2.0f);
    }

    // Clip LED at top (if peak > 0 dB)
    auto clipLed = juce::Rectangle<float>(meterArea.getX() + 1.0f, meterArea.getY() - 10.0f, meterArea.getWidth() - 2.0f, 6.0f);
    if (levelDb >= 0.0f)
    {
        g.setColour(juce::Colour(0xffff1744));
        g.fillRoundedRectangle(clipLed, 2.0f);
        // Clip glow halo
        g.setColour(juce::Colour(0xffff1744).withAlpha(0.6f));
        g.drawRoundedRectangle(clipLed.expanded(1.5f), 2.0f, 1.0f);
    }
    else
    {
        g.setColour(juce::Colour(0xff2d0d14));
        g.fillRoundedRectangle(clipLed, 2.0f);
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

    // Cache active root and scale
    currentRootIndex = rootSelector.getSelectedId() - 1;
    if (currentRootIndex < 0 || currentRootIndex > 11) currentRootIndex = 0;
    currentScaleIndex = scaleSelector.getSelectedId() - 1;
    if (currentScaleIndex < 0 || currentScaleIndex > 7) currentScaleIndex = 0;

    // Smooth needle angle interpolation
    if (isVoiced && currentHz > 50.0f)
    {
        int detectedMidi = audioProcessor.getDetectedMidiNote();
        int semitone = (detectedMidi % 12 + 12) % 12;
        float continuousSemitone = static_cast<float>(semitone) - (currentCentsDeviation / 100.0f);
        float targetAngle = (continuousSemitone / 12.0f) * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;

        // Smooth angle transition
        smoothedPitchAngle += 0.35f * (targetAngle - smoothedPitchAngle);
    }

    repaint();
}

void TPainSupremeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // === LIQUID GLASS BACKGROUND ===
    // 1. Deep cosmic base gradient (dark navy)
    juce::ColourGradient bgGrad(
        juce::Colour(0xff0a0f1e), (float)getWidth() * 0.5f, 0.0f,
        juce::Colour(0xff03050c), (float)getWidth() * 0.5f, (float)getHeight(), false);
    g.setGradientFill(bgGrad);
    g.fillAll();

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

    // === LIQUID GLASS HEADER (Y: 0-82) ===
    auto headerF = juce::Rectangle<float>(0.0f, 0.0f, (float)getWidth(), 82.0f);

    // Header glass base
    juce::ColourGradient hdrBase(
        juce::Colour(0xff141030).withAlpha(0.92f), 0.0f, 0.0f,
        juce::Colour(0xff090618).withAlpha(0.88f), 0.0f, 82.0f, false);
    g.setGradientFill(hdrBase);
    g.fillRect(headerF);

    // Header glass sheen (top half)
    juce::ColourGradient hdrSheen(
        juce::Colour(0xffffffff).withAlpha(0.07f), 0.0f, 0.0f,
        juce::Colours::transparentWhite, 0.0f, 41.0f, false);
    g.setGradientFill(hdrSheen);
    g.fillRect(headerF.withHeight(41.0f));

    // Header specular streak
    juce::ColourGradient hdrSpec(
        juce::Colours::transparentWhite, 0.0f, 2.0f,
        juce::Colour(0xffffffff).withAlpha(0.20f), (float)getWidth() * 0.42f, 2.0f, false);
    hdrSpec.addColour(0.7, juce::Colours::transparentWhite);
    g.setGradientFill(hdrSpec);
    g.fillRect(0, 2, getWidth(), 2);

    // Header neon separator line
    juce::ColourGradient hdrLine(
        juce::Colour(0xff8a2be2), 0.0f, 81.0f,
        juce::Colour(0xffff007f), (float)getWidth(), 81.0f, false);
    g.setGradientFill(hdrLine);
    g.fillRect(0, 80, getWidth(), 2);

    // Header white glass edge
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.07f));
    g.drawHorizontalLine(79, 0.0f, (float)getWidth());

    // Title: SUPREME TUNER
    g.setFont(juce::Font(26.0f, juce::Font::bold));
    juce::ColourGradient titleGrad(
        juce::Colour(0xffe6c2ff), 26.0f, 16.0f,
        juce::Colour(0xffff2a85), 260.0f, 40.0f, false);
    g.setGradientFill(titleGrad);
    g.drawText("SUPREME TUNER", 28, 16, 250, 30, juce::Justification::left);

    // Version Badge: Liquid Glass style
    auto vBadge = juce::Rectangle<float>(286.0f, 20.0f, 128.0f, 22.0f);
    // badge glass base
    juce::ColourGradient vbBase(juce::Colour(0xffff007f).withAlpha(0.18f), vBadge.getX(), vBadge.getY(),
                                juce::Colour(0xffcc0060).withAlpha(0.08f), vBadge.getX(), vBadge.getBottom(), false);
    g.setGradientFill(vbBase);
    g.fillRoundedRectangle(vBadge, 6.0f);
    // badge sheen
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.12f));
    g.fillRoundedRectangle(vBadge.withHeight(11.0f), 6.0f);
    // badge border
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.22f));
    g.drawRoundedRectangle(vBadge, 6.0f, 1.0f);
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffccee));
    g.drawText("REAL TIME v3.5", vBadge.toNearestInt(), juce::Justification::centred);

    // Subtitle
    g.setFont(juce::Font(11.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff9080b8));
    g.drawText("ULTRA-LOW LATENCY VOCAL QUANTIZATION ENGINE", 28, 48, 380, 18, juce::Justification::left);

    // Header selector labels
    g.setFont(juce::Font(11.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffc77dff));
    g.drawText("ARTIST PRESET", 430, 20, 130, 16, juce::Justification::left);
    g.drawText("KEY / ROOT",    690, 20, 100, 16, juce::Justification::left);
    g.drawText("SCALE TYPE",    820, 20, 130, 16, juce::Justification::left);

    // Latency badge: Liquid Glass
    auto latBadge = juce::Rectangle<float>((float)getWidth() - 240.0f, 18.0f, 214.0f, 44.0f);
    // glass base
    juce::ColourGradient latBase(
        juce::Colour(0xff1a1035).withAlpha(0.85f), latBadge.getX(), latBadge.getY(),
        juce::Colour(0xff0d0820).withAlpha(0.80f), latBadge.getX(), latBadge.getBottom(), false);
    g.setGradientFill(latBase);
    g.fillRoundedRectangle(latBadge, 8.0f);
    // glass sheen
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.08f));
    g.fillRoundedRectangle(latBadge.withHeight(22.0f), 8.0f);
    // glass border
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

    // 4. Check if Retune Speed is at 0ms (T-Pain Hard Quantize mode)
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

    // 5. Lower Waves Studio Rack Units (Y: 438, Height: 382)
    int rackY = 438;
    int rackH = 382;

    // Unit 1: NOISE GATE & DYNAMICS (X: 24, W: 290)
    auto dynBox = juce::Rectangle<float>(24.0f, static_cast<float>(rackY), 290.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, dynBox, "NOISE GATE & DYNAMICS", juce::Colour(0xffff007f));

    // Gate Status Badge & LED (Y: rackY + 124 = 562 to 584) - Completely isolated, zero overlap!
    auto gateBadgeArea = juce::Rectangle<int>(38, rackY + 124, 262, 22);
    g.setColour(juce::Colour(0xff120924));
    g.fillRoundedRectangle(gateBadgeArea.toFloat(), 4.0f);
    g.setColour(isGateOpen ? juce::Colour(0xff00f5ff).withAlpha(0.35f) : juce::Colour(0xffff9900).withAlpha(0.35f));
    g.drawRoundedRectangle(gateBadgeArea.toFloat(), 4.0f, 1.0f);

    // Glowing LED Dot
    float ledX = static_cast<float>(gateBadgeArea.getX() + 12);
    float ledY = static_cast<float>(gateBadgeArea.getCentreY());
    juce::Colour ledColor = isGateOpen ? juce::Colour(0xff00f5ff) : juce::Colour(0xffff9900);
    juce::ColourGradient ledGlow(ledColor.withAlpha(0.50f), ledX, ledY, juce::Colours::transparentBlack, ledX + 8.0f, ledY + 8.0f, true);
    g.setGradientFill(ledGlow);
    g.fillEllipse(ledX - 7.0f, ledY - 7.0f, 14.0f, 14.0f);
    g.setColour(ledColor);
    g.fillEllipse(ledX - 3.5f, ledY - 3.5f, 7.0f, 7.0f);
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.fillEllipse(ledX - 1.5f, ledY - 1.5f, 3.0f, 3.0f);

    // Gate text status
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    if (isGateOpen)
    {
        g.setColour(juce::Colour(0xffe0ffff));
        g.drawText("GATE: OPEN (VOICE ACTIVE)", gateBadgeArea.getX() + 24, gateBadgeArea.getY(), gateBadgeArea.getWidth() - 28, gateBadgeArea.getHeight(), juce::Justification::centredLeft);
    }
    else
    {
        g.setColour(juce::Colour(0xffffd599));
        g.drawText("GATE: ATTENUATING -" + juce::String(currentGateGR, 1) + " dB", gateBadgeArea.getX() + 24, gateBadgeArea.getY(), gateBadgeArea.getWidth() - 28, gateBadgeArea.getHeight(), juce::Justification::centredLeft);
    }

    // Dynamic Compression Gain Reduction VU Meter (Hot Pink / Magenta)
    auto grMeterArea = juce::Rectangle<int>(42, rackY + 258, 254, 16);
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
    g.drawText("COMP GAIN REDUCTION", 42, rackY + 278, 140, 16, juce::Justification::left);
    g.drawText("-" + juce::String(currentGR, 1) + " dB", 196, rackY + 278, 100, 16, juce::Justification::right);

    // Unit 2: VOCAL TONE (X: 328, W: 290)
    auto toneBox = juce::Rectangle<float>(328.0f, static_cast<float>(rackY), 290.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, toneBox, "VOCAL TONE ENGINE", juce::Colour(0xff8a2be2));

    // Unit 3: SPACE FX (X: 632, W: 310)
    auto spaceBox = juce::Rectangle<float>(632.0f, static_cast<float>(rackY), 310.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, spaceBox, "SPACE & AMBIENCE", juce::Colour(0xffff00aa));

    // Unit 4: MASTER & STEREO (X: 956, W: 270)
    auto masterBox = juce::Rectangle<float>(956.0f, static_cast<float>(rackY), 270.0f, static_cast<float>(rackH));
    drawWavesRackUnit(g, masterBox, "MASTER & STEREO", juce::Colour(0xffffd700));

    // Live Peak Meter for Output (Right beside the Golden Master Fader)
    auto outMeterArea = juce::Rectangle<float>(1178.0f, static_cast<float>(rackY + 62), 20.0f, 214.0f);
    drawLivePeakMeter(g, outMeterArea, currentOutputMeterDb, "OUT");

    // Live Peak Meter Header Label (Gold)
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffd700));
    g.drawText("OUT", 1172, rackY + 44, 32, 14, juce::Justification::centred);

    // Analog Tape Saturation Active Badge
    auto satBadge = juce::Rectangle<int>(972, rackY + 332, 238, 24);
    g.setColour(juce::Colour(0xffff007f).withAlpha(0.20f));
    g.fillRoundedRectangle(satBadge.toFloat(), 5.0f);
    g.setColour(juce::Colour(0xffff007f));
    g.drawRoundedRectangle(satBadge.toFloat(), 5.0f, 1.2f);
    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffb8dd));
    g.drawText("ANALOG SATURATION: ACTIVE", satBadge, juce::Justification::centred);
}

void TPainSupremeAudioProcessorEditor::drawWavesRackUnit(juce::Graphics& g, juce::Rectangle<float> bounds,
                                                       const juce::String& title, const juce::Colour& headerGlow)
{
    // === LIQUID GLASS PANEL ===
    float cr = 16.0f;

    // 1. Drop shadow (soft dark offset)
    auto shadow = bounds.translated(0.0f, 4.0f).expanded(3.0f, 1.5f);
    juce::ColourGradient shadowGrad(
        juce::Colour(0xff000000).withAlpha(0.45f), shadow.getCentreX(), shadow.getY(),
        juce::Colours::transparentBlack, shadow.getCentreX(), shadow.getBottom(), false);
    g.setGradientFill(shadowGrad);
    g.fillRoundedRectangle(shadow, cr + 2.0f);

    // 2. Glass body base (dark, semi-transparent, slight cool tint)
    juce::ColourGradient glassBase(
        juce::Colour(0xff100c22).withAlpha(0.82f), bounds.getX(), bounds.getY(),
        juce::Colour(0xff070412).withAlpha(0.75f), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(glassBase);
    g.fillRoundedRectangle(bounds, cr);

    // 3. Accent tint layer (accent color bleeding into glass)
    juce::ColourGradient tint(
        headerGlow.withAlpha(0.10f), bounds.getX(), bounds.getY(),
        juce::Colours::transparentBlack, bounds.getX(), bounds.getY() + bounds.getHeight() * 0.45f, false);
    g.setGradientFill(tint);
    g.fillRoundedRectangle(bounds, cr);

    // 4. Frosted glass sheen (white diffuse top half)
    float sheenH = bounds.getHeight() * 0.40f;
    juce::ColourGradient sheen(
        juce::Colour(0xffffffff).withAlpha(0.07f), bounds.getCentreX(), bounds.getY(),
        juce::Colours::transparentWhite, bounds.getCentreX(), bounds.getY() + sheenH, false);
    g.setGradientFill(sheen);
    g.fillRoundedRectangle(bounds.withHeight(sheenH), cr);

    // 5. Specular streak (narrow bright band near top)
    float sPad = bounds.getWidth() * 0.22f;
    juce::ColourGradient streak(
        juce::Colours::transparentWhite, bounds.getX() + sPad, bounds.getY() + 3.5f,
        juce::Colour(0xffffffff).withAlpha(0.26f), bounds.getCentreX(), bounds.getY() + 3.5f, false);
    streak.addColour(1.0, juce::Colours::transparentWhite);
    g.setGradientFill(streak);
    g.fillRoundedRectangle(bounds.getX() + sPad, bounds.getY() + 3.0f,
                           bounds.getWidth() - sPad * 2.0f, 2.5f, 1.2f);

    // 6. Outer glass border (thin white)
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.15f));
    g.drawRoundedRectangle(bounds, cr, 1.0f);

    // 7. Inner border top accent
    auto inner = bounds.reduced(1.2f);
    juce::ColourGradient innerBdr(
        headerGlow.withAlpha(0.32f), inner.getX(), inner.getY(),
        juce::Colour(0xffffffff).withAlpha(0.04f), inner.getX(), inner.getY() + 36.0f, false);
    g.setGradientFill(innerBdr);
    g.drawRoundedRectangle(inner, cr - 1.2f, 0.8f);

    // 8. Title bar (frosted glass strip)
    auto titleBounds = bounds;
    auto titleBar = titleBounds.removeFromTop(34.0f);
    juce::ColourGradient titleBg(
        headerGlow.withAlpha(0.22f), titleBar.getX(), titleBar.getY(),
        headerGlow.withAlpha(0.06f), titleBar.getX(), titleBar.getBottom(), false);
    g.setGradientFill(titleBg);
    g.fillRoundedRectangle(titleBar, cr);  // top of panel

    // Title accent line separator
    g.setColour(headerGlow.withAlpha(0.55f));
    g.fillRect(titleBar.getX() + 16.0f, titleBar.getBottom() - 1.5f, titleBar.getWidth() - 32.0f, 1.5f);

    // 9. Title text (with soft glow pass)
    g.setFont(juce::Font(12.5f, juce::Font::bold));
    // Glow
    g.setColour(headerGlow.withAlpha(0.40f));
    g.drawText(title, titleBar.reduced(16.0f, 0.0f).translated(0.0f, 0.5f), juce::Justification::centredLeft);
    // Sharp
    g.setColour(headerGlow.interpolatedWith(juce::Colours::white, 0.65f));
    g.drawText(title, titleBar.reduced(16.0f, 0.0f), juce::Justification::centredLeft);
}

void TPainSupremeAudioProcessorEditor::drawAntaresPitchWheel(juce::Graphics& g, juce::Rectangle<float> area)
{
    // === LIQUID GLASS PITCH WHEEL PANEL ===
    float cr = 16.0f;

    // Drop shadow
    auto wShadow = area.translated(0.0f, 4.0f).expanded(3.0f, 1.5f);
    juce::ColourGradient wShdGrad(juce::Colour(0xff000000).withAlpha(0.42f), wShadow.getCentreX(), wShadow.getY(),
                                  juce::Colours::transparentBlack, wShadow.getCentreX(), wShadow.getBottom(), false);
    g.setGradientFill(wShdGrad);
    g.fillRoundedRectangle(wShadow, cr + 2.0f);

    // Glass base
    juce::ColourGradient wBase(
        juce::Colour(0xff0e0a22).withAlpha(0.86f), area.getX(), area.getY(),
        juce::Colour(0xff060310).withAlpha(0.80f), area.getX(), area.getBottom(), false);
    g.setGradientFill(wBase);
    g.fillRoundedRectangle(area, cr);

    // Pink tint (top)
    juce::ColourGradient wTint(
        juce::Colour(0xffff007f).withAlpha(0.08f), area.getX(), area.getY(),
        juce::Colours::transparentBlack, area.getX(), area.getY() + area.getHeight() * 0.4f, false);
    g.setGradientFill(wTint);
    g.fillRoundedRectangle(area, cr);

    // Frosted sheen
    float wSheenH = area.getHeight() * 0.38f;
    juce::ColourGradient wSheen(
        juce::Colour(0xffffffff).withAlpha(0.065f), area.getCentreX(), area.getY(),
        juce::Colours::transparentWhite, area.getCentreX(), area.getY() + wSheenH, false);
    g.setGradientFill(wSheen);
    g.fillRoundedRectangle(area.withHeight(wSheenH), cr);

    // Specular streak
    float wsPad = area.getWidth() * 0.22f;
    juce::ColourGradient wsStreak(
        juce::Colours::transparentWhite, area.getX() + wsPad, area.getY() + 3.5f,
        juce::Colour(0xffffffff).withAlpha(0.24f), area.getCentreX(), area.getY() + 3.5f, false);
    wsStreak.addColour(1.0, juce::Colours::transparentWhite);
    g.setGradientFill(wsStreak);
    g.fillRoundedRectangle(area.getX() + wsPad, area.getY() + 3.0f, area.getWidth() - wsPad * 2.0f, 2.5f, 1.2f);

    // Glass outer border
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.14f));
    g.drawRoundedRectangle(area, cr, 1.0f);

    // Inner accent border
    juce::ColourGradient wInner(
        juce::Colour(0xffff007f).withAlpha(0.28f), area.getX(), area.getY(),
        juce::Colour(0xffffffff).withAlpha(0.04f), area.getX(), area.getY() + 38.0f, false);
    g.setGradientFill(wInner);
    g.drawRoundedRectangle(area.reduced(1.2f), cr - 1.2f, 0.8f);

    // Panel header text
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffff007f).withAlpha(0.80f));
    g.drawText("REAL-TIME CHROMATIC PITCH WHEEL", (int)area.getX() + 18, (int)area.getY() + 10, 360, 20, juce::Justification::left);

    // Wheel Geometry: cy moved up to 144 so bottom node clears the cents meter
    float cx = area.getCentreX();
    float cy = area.getY() + 144.0f;
    float outerRadius = 104.0f;
    float innerRadius = 56.0f;

    // 1. Concentric Background Tracks & Neon Orbital Rings
    g.setColour(juce::Colour(0xff22143e));
    g.drawEllipse(cx - outerRadius, cy - outerRadius, outerRadius * 2.0f, outerRadius * 2.0f, 1.8f);
    g.drawEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.2f);

    // Subtle radial tick marks (every 10 degrees)
    for (int deg = 0; deg < 360; deg += 10)
    {
        float rad = deg * juce::MathConstants<float>::pi / 180.0f;
        float r1 = outerRadius - ((deg % 30 == 0) ? 10.0f : 5.0f);
        float r2 = outerRadius - 1.0f;
        g.setColour((deg % 30 == 0) ? juce::Colour(0xff9d4edd).withAlpha(0.5f) : juce::Colour(0xff482d73).withAlpha(0.4f));
        g.drawLine(cx + r1 * std::cos(rad), cy + r1 * std::sin(rad),
                   cx + r2 * std::cos(rad), cy + r2 * std::sin(rad), (deg % 30 == 0) ? 1.5f : 1.0f);
    }

    // 2. Scale Mask for Active Root Note & Scale Type
    VeviAudio::ScaleType scaleType = static_cast<VeviAudio::ScaleType>(currentScaleIndex);
    uint16_t scaleMask = VeviAudio::PitchEngine::getScaleMask(scaleType);

    // Find currently active detected note index
    int activeDetectedIndex = -1;
    if (isVoiced && currentHz > 50.0f)
    {
        int detectedMidi = audioProcessor.getDetectedMidiNote();
        activeDetectedIndex = (detectedMidi % 12 + 12) % 12;
    }

    // 3. Draw 12 Chromatic Note Nodes radially around the circle (C at top = -90 deg)
    for (int i = 0; i < 12; ++i)
    {
        float angle = (static_cast<float>(i) / 12.0f) * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
        float noteRadius = outerRadius;
        float nx = cx + noteRadius * std::cos(angle);
        float ny = cy + noteRadius * std::sin(angle);

        // Check if note is in active scale
        int semitoneRelRoot = ((i - currentRootIndex) % 12 + 12) % 12;
        bool inScale = (scaleMask & (1 << semitoneRelRoot)) != 0;
        bool isDetected = (activeDetectedIndex == i);

        float nodeW = isDetected ? 34.0f : 28.0f;
        auto nodeRect = juce::Rectangle<float>(nx - nodeW * 0.5f, ny - nodeW * 0.5f, nodeW, nodeW);

        if (isDetected)
        {
            // Glowing Hot Pink Flare for Active Sung Note!
            juce::ColourGradient flare(juce::Colour(0xffff007f), nx, ny,
                                       juce::Colour(0xffff007f).withAlpha(0.0f), nx + 28.0f, ny + 28.0f, true);
            g.setGradientFill(flare);
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
            // In Scale: Illuminated Neon Violet
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
            // Out of Scale: Dimmed Dark Slate
            g.setColour(juce::Colour(0xff140c24));
            g.fillEllipse(nodeRect);
            g.setColour(juce::Colour(0xff33214e));
            g.drawEllipse(nodeRect, 1.0f);

            g.setFont(juce::Font(9.5f, juce::Font::plain));
            g.setColour(juce::Colour(0xff675482));
            g.drawText(VeviAudio::NoteNames[i], nodeRect, juce::Justification::centred);
        }
    }

    // 4. Real-Time Pitch Pointer / Laser Needle
    if (isVoiced && currentHz > 50.0f)
    {
        float needleRadius = outerRadius - 16.0f;
        float tipX = cx + needleRadius * std::cos(smoothedPitchAngle);
        float tipY = cy + needleRadius * std::sin(smoothedPitchAngle);

        juce::Path needle;
        needle.startNewSubPath(cx, cy);
        needle.lineTo(tipX, tipY);

        // Glowing needle line
        g.setColour(juce::Colour(0xffff007f).withAlpha(0.45f));
        g.strokePath(needle, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(juce::Colour(0xffffffff));
        g.strokePath(needle, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Tip bead
        g.setColour(juce::Colour(0xffff007f));
        g.fillEllipse(tipX - 4.0f, tipY - 4.0f, 8.0f, 8.0f);
        g.setColour(juce::Colours::white);
        g.fillEllipse(tipX - 1.8f, tipY - 1.8f, 3.6f, 3.6f);
    }

    // 5. Central Hub Display — LIQUID GLASS ORB
    auto hubRect = juce::Rectangle<float>(cx - innerRadius + 4.0f, cy - innerRadius + 4.0f,
                                          (innerRadius - 4.0f) * 2.0f, (innerRadius - 4.0f) * 2.0f);
    // Glass base
    juce::ColourGradient hubBase(
        juce::Colour(0xff0e0820).withAlpha(0.90f), hubRect.getX() + hubRect.getWidth() * 0.35f, hubRect.getY() + hubRect.getHeight() * 0.25f,
        juce::Colour(0xff060412).withAlpha(0.95f), hubRect.getRight(), hubRect.getBottom(), true);
    g.setGradientFill(hubBase);
    g.fillEllipse(hubRect);
    // Glass sheen top-left
    juce::ColourGradient hubSheen(
        juce::Colour(0xffffffff).withAlpha(0.16f), hubRect.getX() + hubRect.getWidth() * 0.22f, hubRect.getY() + hubRect.getHeight() * 0.15f,
        juce::Colours::transparentWhite, hubRect.getCentreX(), hubRect.getCentreY(), true);
    g.setGradientFill(hubSheen);
    g.fillEllipse(hubRect);
    // Outer glass border
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.20f));
    g.drawEllipse(hubRect, 1.2f);
    // Inner violet accent
    g.setColour(juce::Colour(0xff8a2be2).withAlpha(0.45f));
    g.drawEllipse(hubRect.reduced(1.8f), 0.8f);

    if (isVoiced && currentHz > 50.0f)
    {
        // Big Target Note
        g.setFont(juce::Font(26.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xffff007f));
        g.drawText(currentTargetNote, (int)hubRect.getX(), (int)hubRect.getY() + 12, (int)hubRect.getWidth(), 28, juce::Justification::centred);

        // Frequency readout
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xffe6c2ff));
        g.drawText(juce::String(currentHz, 1) + " Hz", (int)hubRect.getX(), (int)hubRect.getY() + 42, (int)hubRect.getWidth(), 16, juce::Justification::centred);

        // Cents offset text
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

    // 6. Cents Meter Scale Under Wheel
    // Fixed Y: placed at area.getY() + 292 for 30px clearance below the F# node
    float meterY = area.getY() + 292.0f;
    auto meterArea = juce::Rectangle<float>(cx - 120.0f, meterY, 240.0f, 7.0f);
    g.setColour(juce::Colour(0xff180e2d));
    g.fillRoundedRectangle(meterArea, 3.5f);
    g.setColour(juce::Colour(0xff43266f));
    g.drawRoundedRectangle(meterArea, 3.5f, 1.0f);

    // Center Zero Marker
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

    // Scale Labels: "0c" placed below meter with comfortable gap
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff947dae));
    g.drawText("-50c", (int)(meterArea.getX() - 34.0f), (int)(meterArea.getY() - 3.0f), 30, 14, juce::Justification::right);
    g.drawText("0c",   (int)(cx - 12.0f), (int)(meterArea.getBottom() + 3.0f), 24, 14, juce::Justification::centred);
    g.drawText("+50c", (int)(meterArea.getRight() + 4.0f), (int)(meterArea.getY() - 3.0f), 34, 14, juce::Justification::left);
}

void TPainSupremeAudioProcessorEditor::resized()
{
    // Top Selectors
    presetSelector.setBounds(430, 42, 240, 28);
    rootSelector.setBounds(690, 42, 115, 28);
    scaleSelector.setBounds(820, 42, 145, 28);

    // Middle Stage Y: 96, Height: 328
    // Middle Left: Tuning Knobs (Box X: 24, W: 290)
    retuneSpeedSlider.setBounds(38, 130, 130, 134);
    speedLabel.setBounds(30, 268, 146, 16);

    transitionSlider.setBounds(184, 142, 108, 122);
    transSpeedLabel.setBounds(176, 268, 124, 16);

    // Middle Right: Centering & Variation Knobs (Box X: 936, W: 290)
    centeringSlider.setBounds(getWidth() - 274, 142, 108, 122);
    centerLabel.setBounds(getWidth() - 282, 268, 124, 16);

    variationSlider.setBounds(getWidth() - 146, 142, 108, 122);
    varLabel.setBounds(getWidth() - 154, 268, 124, 16);

    // Lower Rack Units (Y: 438, Height: 382)
    int rackY = 438;

    // Unit 1: NOISE GATE & DYNAMICS (X: 24, W: 290)
    // Row 1: Noise Gate (3 Rotary Purple Knobs: Thresh, Attack, Release)
    int gateKnobY = rackY + 38;
    int gateKnobW = 76;
    int gateKnobH = 64;
    gateThreshSlider.setBounds(33, gateKnobY, gateKnobW, gateKnobH);
    gateThrLabel.setBounds(27, gateKnobY + gateKnobH + 2, 88, 14);

    gateAttackSlider.setBounds(121, gateKnobY, gateKnobW, gateKnobH);
    gateAtkLabel.setBounds(115, gateKnobY + gateKnobH + 2, 88, 14);

    gateReleaseSlider.setBounds(209, gateKnobY, gateKnobW, gateKnobH);
    gateRelLabel.setBounds(203, gateKnobY + gateKnobH + 2, 88, 14);

    // Row 2: Dynamics (2 Rotary Purple Knobs: Transients & Compression)
    int dynKnobY = rackY + 154;
    int dynKnobW = 96;
    int dynKnobH = 76;
    transientsSlider.setBounds(48, dynKnobY, dynKnobW, dynKnobH);
    transLabel.setBounds(44, dynKnobY + dynKnobH + 2, 104, 14);

    compressionSlider.setBounds(190, dynKnobY, dynKnobW, dynKnobH);
    compLabel.setBounds(186, dynKnobY + dynKnobH + 2, 104, 14);

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
    // Row 1: Reverb
    int r1Y = rackY + 46;
    int r1H = 80;
    reverbMixSlider.setBounds(672, r1Y, 82, r1H);
    revMixLabel.setBounds(662, r1Y + r1H + 2, 102, 14);

    reverbSizeSlider.setBounds(788, r1Y, 82, r1H);
    revSizeLabel.setBounds(778, r1Y + r1H + 2, 102, 14);

    // Row 2: Delay
    int r2Y = rackY + 160;
    int r2H = 80;
    delayMixSlider.setBounds(642, r2Y, 82, r2H);
    dlyMixLabel.setBounds(632, r2Y + r2H + 2, 102, 14);

    delayTimeSlider.setBounds(746, r2Y, 82, r2H);
    dlyTimeLabel.setBounds(736, r2Y + r2H + 2, 102, 14);

    delayFeedbackSlider.setBounds(850, r2Y, 82, r2H);
    dlyFbLabel.setBounds(840, r2Y + r2H + 2, 102, 14);

    // Unit 4: GAIN STAGING & MASTER (X: 956, W: 270)
    // Left: 3 Rotary Purple Knobs (Input Gain, Stereo Width, Dry/Wet Mix)
    inputGainSlider.setBounds(972, rackY + 40, 88, 66);
    inputGainLabel.setBounds(966, rackY + 108, 100, 14);

    stereoWidthSlider.setBounds(972, rackY + 130, 88, 66);
    stereoWidthLabel.setBounds(966, rackY + 198, 100, 14);

    mixSlider.setBounds(972, rackY + 220, 88, 66);
    mixLabel.setBounds(966, rackY + 288, 100, 14);

    // Right: THE GOLDEN MASTER FADER (Console 24K Gold)
    outLabel.setBounds(1084, rackY + 40, 80, 16);
    outputSlider.setBounds(1084, rackY + 58, 80, 248);
}
