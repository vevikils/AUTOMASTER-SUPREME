#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Supreme Waves Look & Feel: Ultra-Premium Hardware Rack Style
//==============================================================================

SupremeWavesLookAndFeel::SupremeWavesLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff09070e));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xee0e0a20));
    setColour(juce::PopupMenu::textColourId, juce::Colour(0xfff0eaff));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xffff007f).withAlpha(0.28f));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
}

void SupremeWavesLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPosProportional, float rotaryStartAngle,
                                              float rotaryEndAngle, juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    auto currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // 1. Intense Circular Purple / Magenta LED Halo Light Ring (Reference Image)
    float haloR = radius - 4.0f;
    juce::Colour neonPurple = juce::Colour(0xffc084fc);
    juce::Colour neonMagenta = juce::Colour(0xffe879f9);
    juce::Colour deepPurple = juce::Colour(0xffa855f7);

    // Diffuse ambient glow on chassis
    juce::ColourGradient ambientHalo(deepPurple.withAlpha(0.35f), centre.x, centre.y,
                                     juce::Colours::transparentBlack, centre.x + haloR + 10.0f, centre.y + haloR + 10.0f, true);
    g.setGradientFill(ambientHalo);
    g.fillEllipse(centre.x - haloR - 10.0f, centre.y - haloR - 10.0f, (haloR + 10.0f) * 2.0f, (haloR + 10.0f) * 2.0f);

    // Intense glowing LED ring around base
    g.setColour(neonMagenta.withAlpha(0.70f));
    g.drawEllipse(centre.x - haloR, centre.y - haloR, haloR * 2.0f, haloR * 2.0f, 3.5f);
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.drawEllipse(centre.x - haloR, centre.y - haloR, haloR * 2.0f, haloR * 2.0f, 1.2f);

    // 2. Dark Carbon / Titanium Knob Cap
    float capRadius = haloR - 5.0f;
    if (capRadius > 4.0f)
    {
        auto capX = centre.x - capRadius;
        auto capY = centre.y - capRadius;
        auto capD = capRadius * 2.0f;

        // Outer knurled textured ring (micro-notches around the circumference)
        g.setColour(juce::Colour(0xff120e1c));
        g.fillEllipse(capX, capY, capD, capD);

        for (int deg = 0; deg < 360; deg += 12)
        {
            float rad = deg * juce::MathConstants<float>::pi / 180.0f;
            float r1 = capRadius - 3.5f;
            float r2 = capRadius - 0.5f;
            g.setColour((deg % 24 == 0) ? juce::Colour(0xff4a3a60) : juce::Colour(0xff251c33));
            g.drawLine(centre.x + r1 * std::cos(rad), centre.y + r1 * std::sin(rad),
                       centre.x + r2 * std::cos(rad), centre.y + r2 * std::sin(rad), 1.2f);
        }

        // Inner cap body
        float innerCapR = capRadius - 4.0f;
        auto innerX = centre.x - innerCapR;
        auto innerY = centre.y - innerCapR;
        auto innerD = innerCapR * 2.0f;

        juce::ColourGradient capBase(
            juce::Colour(0xff282236), centre.x - innerCapR * 0.4f, centre.y - innerCapR * 0.5f,
            juce::Colour(0xff100c18), centre.x + innerCapR * 0.5f, centre.y + innerCapR * 0.6f, true);
        g.setGradientFill(capBase);
        g.fillEllipse(innerX, innerY, innerD, innerD);

        // Diffuse top-left specular sheen
        juce::ColourGradient capSheen(
            juce::Colour(0xffffffff).withAlpha(0.24f), centre.x - innerCapR * 0.35f, centre.y - innerCapR * 0.45f,
            juce::Colours::transparentWhite, centre.x + innerCapR * 0.15f, centre.y + innerCapR * 0.10f, true);
        g.setGradientFill(capSheen);
        g.fillEllipse(innerX, innerY, innerD, innerD);

        // Beveled metallic rim
        g.setColour(juce::Colour(0xff574570));
        g.drawEllipse(innerX, innerY, innerD, innerD, 1.2f);

        // 3. Precision Pointer Notch (White line with magenta glow)
        float r1 = innerCapR * 0.28f;
        float r2 = innerCapR * 0.88f;
        float sinA = std::sin(currentAngle);
        float cosA = -std::cos(currentAngle); // 0 at 12 o'clock

        juce::Line<float> needle(centre.x + r1 * sinA, centre.y + r1 * cosA,
                                centre.x + r2 * sinA, centre.y + r2 * cosA);

        g.setColour(neonMagenta.withAlpha(0.80f));
        g.drawLine(needle, 3.8f);
        g.setColour(juce::Colours::white);
        g.drawLine(needle, 1.8f);

        // Center jewel orb
        g.setColour(deepPurple);
        g.fillEllipse(centre.x - 2.5f, centre.y - 2.5f, 5.0f, 5.0f);
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.fillEllipse(centre.x - 1.0f, centre.y - 1.0f, 2.0f, 2.0f);
    }
}

// THE 24K GOLD MASTER FADER THUMB
void SupremeWavesLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float minSliderPos, float maxSliderPos,
                                              const juce::Slider::SliderStyle /*style*/, juce::Slider& /*slider*/)
{
    float cx = static_cast<float>(x) + static_cast<float>(width) * 0.5f;

    // Solid Gold Console Fader Handle (Matching Reference Image)
    float thumbW = 46.0f;
    float thumbH = 26.0f;
    float thumbY = sliderPos - thumbH * 0.5f;
    auto thumbRect = juce::Rectangle<float>(cx - thumbW * 0.5f, thumbY, thumbW, thumbH);

    // Soft drop shadow cast on gold plate
    g.setColour(juce::Colour(0x66000000));
    g.fillRoundedRectangle(thumbRect.translated(0.0f, 3.0f), 3.0f);

    // Fader cap body: 24K Brushed Gold metallic gradient
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
    g.fillRect(thumbRect.getX() + 5.0f, thumbY + 4.5f, thumbRect.getWidth() - 10.0f, 1.2f);
    g.fillRect(thumbRect.getX() + 5.0f, thumbY + 20.0f, thumbRect.getWidth() - 10.0f, 1.2f);
    g.setColour(juce::Colour(0xffffe680));
    g.fillRect(thumbRect.getX() + 5.0f, thumbY + 5.7f, thumbRect.getWidth() - 10.0f, 0.6f);
    g.fillRect(thumbRect.getX() + 5.0f, thumbY + 21.2f, thumbRect.getWidth() - 10.0f, 0.6f);

    // Illuminated Horizontal Center Notch Line (Bright White Core with Gold Halo)
    g.setColour(juce::Colour(0xffffd700).withAlpha(0.65f));
    g.fillRect(thumbRect.getX() + 2.0f, thumbY + 11.5f, thumbRect.getWidth() - 4.0f, 3.0f);
    g.setColour(juce::Colours::white);
    g.fillRect(thumbRect.getX() + 4.0f, thumbY + 12.2f, thumbRect.getWidth() - 8.0f, 1.5f);
}

void SupremeWavesLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                          int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                          juce::ComboBox& box)
{
    bool focused = box.hasKeyboardFocus(false) || isButtonDown;
    auto area = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f);
    float cr = 6.0f;

    // Dark titanium glass base
    juce::ColourGradient base(
        juce::Colour(0xff161124).withAlpha(0.92f), 0.0f, 0.0f,
        juce::Colour(0xff0a0712).withAlpha(0.88f), 0.0f, (float)height, false);
    g.setGradientFill(base);
    g.fillRoundedRectangle(area, cr);

    // Glass sheen
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.08f));
    g.fillRoundedRectangle(area.withHeight(height * 0.45f), cr);

    // Border
    g.setColour(focused ? juce::Colour(0xff00f5ff).withAlpha(0.80f) : juce::Colour(0xffffffff).withAlpha(0.18f));
    g.drawRoundedRectangle(area, cr, 1.1f);

    // Chevron
    juce::Path arrow;
    float ax = width - 16.0f, ay = height * 0.5f - 2.0f;
    arrow.startNewSubPath(ax, ay);
    arrow.lineTo(ax + 4.5f, ay + 4.5f);
    arrow.lineTo(ax + 9.0f, ay);
    g.setColour(focused ? juce::Colour(0xff00f5ff) : juce::Colour(0xffc77dff));
    g.strokePath(arrow, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
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
    presetLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    presetLabel.setJustificationType(juce::Justification::centred);
    presetLabel.setColour(juce::Label::textColourId, juce::Colour(0xffcbd5e1));
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

    // The 4 Bottom Studio Knobs (SPEED, AMOUNT, SENS, MIX)
    configureKnob(retuneSpeedSlider, speedLabel, "SPEED", " ms");
    configureKnob(centeringSlider, centerLabel, "AMOUNT", " %");
    configureKnob(gateThreshSlider, gateThrLabel, "SENS", " dB");
    configureKnob(mixSlider, mixLabel, "MIX", " %");

    // The 24K Gold Master Level Fader
    configureVerticalFader(outputSlider, outLabel, "3.6 MASTER LEVEL", " dB");

    // Secondary controls initialized and attached so DSP presets work 100%
    configureKnob(transitionSlider, transSpeedLabel, "TRANSITION", " ms");
    configureKnob(variationSlider, varLabel, "VARIATION", " %");
    configureVerticalFader(transientsSlider, transLabel, "TRANSIENTS", " %");
    configureVerticalFader(compressionSlider, compLabel, "COMPRESSION", " %");
    configureKnob(lowCutSlider, lowCutLabel, "LOW CUT", " Hz");
    configureKnob(bodySlider, bodyLabel, "BODY", " dB");
    configureKnob(airSlider, airLabel, "AIR SHEEN", " dB");
    configureKnob(reverbMixSlider, revMixLabel, "REV MIX", " %");
    configureKnob(reverbSizeSlider, revSizeLabel, "REV SIZE", " %");
    configureKnob(delayMixSlider, dlyMixLabel, "DLY MIX", " %");
    configureKnob(delayTimeSlider, dlyTimeLabel, "DLY TIME", " ms");
    configureKnob(delayFeedbackSlider, dlyFbLabel, "FEEDBACK", " %");
    configureVerticalFader(inputGainSlider, inputGainLabel, "INPUT", " dB");
    configureKnob(stereoWidthSlider, stereoWidthLabel, "STEREO WIDTH", " %");
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

    // Standard 19" Hardware Rack Dimensions
    setSize(1220, 640);
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
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 16);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff0e8ff));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(12.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffcbd5e1));
    addAndMakeVisible(label);
}

void TPainSupremeAudioProcessorEditor::configureVerticalFader(juce::Slider& slider, juce::Label& label,
                                                             const juce::String& text, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setTextValueSuffix(suffix);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(11.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffffd700));
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

    // Standard VU ballistic dynamics (300 ms response with smooth needle movement)
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
// 19" Hardware Rack Ears with Screw Cutouts & Vertical Neon LED Light Bars
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawHardwareRackEars(juce::Graphics& g, int w, int h)
{
    float earW = 44.0f;
    auto leftEar = juce::Rectangle<float>(0.0f, 0.0f, earW, (float)h);
    auto rightEar = juce::Rectangle<float>((float)w - earW, 0.0f, earW, (float)h);

    // Metal gradients for rack ears
    juce::ColourGradient leftEarGrad(juce::Colour(0xff181422), 0.0f, 0.0f, juce::Colour(0xff0d0a14), earW, 0.0f, false);
    g.setGradientFill(leftEarGrad);
    g.fillRect(leftEar);

    juce::ColourGradient rightEarGrad(juce::Colour(0xff0d0a14), (float)w - earW, 0.0f, juce::Colour(0xff181422), (float)w, 0.0f, false);
    g.setGradientFill(rightEarGrad);
    g.fillRect(rightEar);

    // 4 Oval Screw Cutouts per ear
    float cutoutYPositions[] = { h * 0.12f, h * 0.36f, h * 0.64f, h * 0.88f };
    for (float cyPos : cutoutYPositions)
    {
        // Left Ear Cutout
        auto lCutout = juce::Rectangle<float>(12.0f, cyPos - 12.0f, 18.0f, 24.0f);
        g.setColour(juce::Colour(0xff050308));
        g.fillRoundedRectangle(lCutout, 9.0f);
        g.setColour(juce::Colour(0xff3f354c));
        g.drawRoundedRectangle(lCutout, 9.0f, 1.2f);

        // Right Ear Cutout
        auto rCutout = juce::Rectangle<float>((float)w - 30.0f, cyPos - 12.0f, 18.0f, 24.0f);
        g.setColour(juce::Colour(0xff050308));
        g.fillRoundedRectangle(rCutout, 9.0f);
        g.setColour(juce::Colour(0xff3f354c));
        g.drawRoundedRectangle(rCutout, 9.0f, 1.2f);
    }

    // Vertical Neon LED Light Bars along the inner fold of each ear
    auto lBar = juce::Rectangle<float>(earW - 3.5f, 10.0f, 3.5f, (float)h - 20.0f);
    auto rBar = juce::Rectangle<float>((float)w - earW, 10.0f, 3.5f, (float)h - 20.0f);

    juce::Colour neonPurple = juce::Colour(0xffc084fc);
    juce::Colour neonMagenta = juce::Colour(0xffe879f9);

    // Left neon bar glow
    juce::ColourGradient lGlow(neonMagenta.withAlpha(0.40f), lBar.getX(), 0.0f, juce::Colours::transparentBlack, lBar.getX() - 14.0f, 0.0f, false);
    g.setGradientFill(lGlow);
    g.fillRect(lBar.expanded(14.0f, 0.0f));

    g.setColour(neonPurple);
    g.fillRoundedRectangle(lBar, 1.75f);
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(lBar.reduced(1.0f, 1.0f), 1.0f);

    // Right neon bar glow
    juce::ColourGradient rGlow(neonMagenta.withAlpha(0.40f), rBar.getRight(), 0.0f, juce::Colours::transparentBlack, rBar.getRight() + 14.0f, 0.0f, false);
    g.setGradientFill(rGlow);
    g.fillRect(rBar.expanded(14.0f, 0.0f));

    g.setColour(neonPurple);
    g.fillRoundedRectangle(rBar, 1.75f);
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(rBar.reduced(1.0f, 1.0f), 1.0f);
}

//==============================================================================
// 1/4" Golden Headphone/Aux Jack
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawQuarterInchJack(juce::Graphics& g, float cx, float cy, float r)
{
    g.setColour(juce::Colour(0x66000000));
    g.fillEllipse(cx - r - 2.0f, cy - r - 2.0f, (r + 2.0f) * 2.0f, (r + 2.0f) * 2.0f);

    juce::Path hex;
    for (int i = 0; i < 6; ++i)
    {
        float angle = i * juce::MathConstants<float>::twoPi / 6.0f;
        float hx = cx + r * std::cos(angle);
        float hy = cy + r * std::sin(angle);
        if (i == 0) hex.startNewSubPath(hx, hy);
        else hex.lineTo(hx, hy);
    }
    hex.closeSubPath();

    juce::ColourGradient goldNut(juce::Colour(0xfffff2a3), cx - r, cy - r, juce::Colour(0xff8a6012), cx + r, cy + r, false);
    g.setGradientFill(goldNut);
    g.fillPath(hex);

    g.setColour(juce::Colour(0xff573a06));
    g.strokePath(hex, juce::PathStrokeType(1.2f));

    float innerR = r * 0.52f;
    g.setColour(juce::Colour(0xff08060c));
    g.fillEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f);

    g.setColour(juce::Colour(0xffd4af37));
    g.drawEllipse(cx - innerR, cy - innerR, innerR * 2.0f, innerR * 2.0f, 1.0f);

    g.setColour(juce::Colour(0xff1f1807));
    g.fillEllipse(cx - innerR * 0.6f, cy - innerR * 0.6f, innerR * 1.2f, innerR * 1.2f);
}

//==============================================================================
// Dual Vintage Analog VU Meters (Curved Cream/Amber Backlit Dials & Red Needles)
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawAnalogVUMeter(juce::Graphics& g, juce::Rectangle<float> meterBox, float needleNorm, const juce::String& /*title*/)
{
    float cr = 6.0f;

    g.setColour(juce::Colour(0xff0e0a16));
    g.fillRoundedRectangle(meterBox, cr);
    g.setColour(juce::Colour(0xff2d2238));
    g.drawRoundedRectangle(meterBox, cr, 1.0f);

    auto dialBox = meterBox.reduced(3.0f);

    // Warm incandescent dial face
    juce::ColourGradient dialFace(
        juce::Colour(0xff331908), dialBox.getCentreX(), dialBox.getY(),
        juce::Colour(0xff170b04), dialBox.getCentreX(), dialBox.getBottom(), false);
    g.setGradientFill(dialFace);
    g.fillRoundedRectangle(dialBox, cr - 1.0f);

    // Warm amber backlit glow
    juce::ColourGradient amberGlow(
        juce::Colour(0xffff8c00).withAlpha(0.32f), dialBox.getCentreX(), dialBox.getY() + 8.0f,
        juce::Colours::transparentBlack, dialBox.getCentreX(), dialBox.getY() + 60.0f, true);
    g.setGradientFill(amberGlow);
    g.fillRoundedRectangle(dialBox, cr - 1.0f);

    float pivotX = dialBox.getCentreX();
    float pivotY = dialBox.getBottom() + 18.0f;
    float arcRadius = 88.0f;

    float startAng = -48.0f * juce::MathConstants<float>::pi / 180.0f;
    float zeroAng = 12.0f * juce::MathConstants<float>::pi / 180.0f;
    float endAng = 38.0f * juce::MathConstants<float>::pi / 180.0f;

    // Normal zone (-20 to 0)
    juce::Path normalArc;
    normalArc.addCentredArc(pivotX, pivotY, arcRadius, arcRadius, 0.0f, startAng, zeroAng, true);
    g.setColour(juce::Colour(0xffffe2b3));
    g.strokePath(normalArc, juce::PathStrokeType(1.6f));

    // Overload red zone (0 to +3 dB)
    juce::Path redArc;
    redArc.addCentredArc(pivotX, pivotY, arcRadius, arcRadius, 0.0f, zeroAng, endAng, true);
    g.setColour(juce::Colour(0xffef4444));
    g.strokePath(redArc, juce::PathStrokeType(2.4f));

    // Scale tick marks
    struct VUTick { float valNorm; const char* txt; bool isRed; };
    VUTick ticks[] = {
        { 0.0f, "-20", false },
        { 0.25f, "-10", false },
        { 0.42f, "-7", false },
        { 0.58f, "-5", false },
        { 0.72f, "-3", false },
        { 0.85f, "-1", false },
        { 0.90f, "0", true },
        { 0.95f, "+1", true },
        { 1.0f, "+3", true }
    };

    g.setFont(juce::Font(7.5f, juce::Font::bold));
    for (const auto& tk : ticks)
    {
        float angle = startAng + tk.valNorm * (endAng - startAng);
        float r1 = arcRadius - (tk.isRed ? 4.0f : 3.0f);
        float r2 = arcRadius + 1.0f;

        float x1 = pivotX + r1 * std::sin(angle);
        float y1 = pivotY - r1 * std::cos(angle);
        float x2 = pivotX + r2 * std::sin(angle);
        float y2 = pivotY - r2 * std::cos(angle);

        g.setColour(tk.isRed ? juce::Colour(0xffef4444) : juce::Colour(0xffffe2b3));
        g.drawLine(x1, y1, x2, y2, tk.isRed ? 1.5f : 1.0f);
    }

    // VU Logo
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffffe2b3).withAlpha(0.85f));
    g.drawText("VU", (int)dialBox.getX(), (int)dialBox.getY() + 38, (int)dialBox.getWidth(), 14, juce::Justification::centred);

    // Dynamic Needle
    float clampedNorm = juce::jlimit(0.0f, 1.0f, needleNorm);
    float needleAngle = startAng + clampedNorm * (endAng - startAng);
    float needleLen = arcRadius + 2.0f;

    float tipX = pivotX + needleLen * std::sin(needleAngle);
    float tipY = pivotY - needleLen * std::cos(needleAngle);

    g.setColour(juce::Colour(0x55000000));
    g.drawLine(pivotX + 1.5f, pivotY + 1.0f, tipX + 1.5f, tipY + 1.0f, 2.0f);

    g.setColour(juce::Colour(0xffff2222));
    g.drawLine(pivotX, pivotY, tipX, tipY, 1.8f);
    g.setColour(juce::Colour(0xffff8888));
    g.drawLine(pivotX, pivotY, tipX, tipY, 0.8f);

    g.setColour(juce::Colour(0xff120c18));
    g.fillEllipse(pivotX - 6.0f, pivotY - 6.0f, 12.0f, 12.0f);
    g.setColour(juce::Colour(0xffd4af37));
    g.drawEllipse(pivotX - 6.0f, pivotY - 6.0f, 12.0f, 12.0f, 1.0f);
}

void TPainSupremeAudioProcessorEditor::drawDualAnalogVUMeters(juce::Graphics& g, juce::Rectangle<float> area)
{
    // Outer acrylic display window
    g.setColour(juce::Colour(0xff090610));
    g.fillRoundedRectangle(area, 8.0f);

    juce::ColourGradient frameGrad(
        juce::Colour(0xff3b2d4c), area.getX(), area.getY(),
        juce::Colour(0xff120d1c), area.getRight(), area.getBottom(), false);
    g.setGradientFill(frameGrad);
    g.drawRoundedRectangle(area, 8.0f, 1.8f);

    juce::ColourGradient sheen(
        juce::Colour(0xffffffff).withAlpha(0.08f), area.getCentreX(), area.getY(),
        juce::Colours::transparentWhite, area.getCentreX(), area.getY() + 30.0f, false);
    g.setGradientFill(sheen);
    g.fillRoundedRectangle(area.withHeight(30.0f), 8.0f);

    float meterW = 152.0f;
    float meterH = 142.0f;
    float meterY = area.getY() + 12.0f;

    auto lMeterBox = juce::Rectangle<float>(area.getX() + 14.0f, meterY, meterW, meterH);
    auto rMeterBox = juce::Rectangle<float>(area.getX() + 184.0f, meterY, meterW, meterH);

    drawAnalogVUMeter(g, lMeterBox, smoothedLeftVU, "IN");
    drawAnalogVUMeter(g, rMeterBox, smoothedRightVU, "OUT");

    // Bottom digital scale display bar inside acrylic window
    auto statusBox = juce::Rectangle<float>(area.getX() + 14.0f, area.getY() + 164.0f, area.getWidth() - 28.0f, 32.0f);
    g.setColour(juce::Colour(0xff050308));
    g.fillRoundedRectangle(statusBox, 4.0f);
    g.setColour(juce::Colour(0xff2a1b3d));
    g.drawRoundedRectangle(statusBox, 4.0f, 1.0f);

    juce::String autoTuneText = juce::String("AUTO-TUNE: ") + VeviAudio::NoteNames[currentRootIndex] + " " + scaleSelector.getText();
    g.setFont(juce::Font(11.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00f5ff));
    g.drawText(autoTuneText, statusBox.toNearestInt(), juce::Justification::centred);

    // Serigraphy under acrylic window
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff94a3b8));
    g.drawText("REAL TIME PITCH CORRECTION", (int)area.getX(), (int)area.getBottom() + 6, (int)area.getWidth(), 16, juce::Justification::centred);
}

//==============================================================================
// Chromatic Pitch Wheel with Circular Segmented Cyan LED Ring
//==============================================================================
void TPainSupremeAudioProcessorEditor::drawSegmentedPitchWheel(juce::Graphics& g, juce::Rectangle<float> area)
{
    float cx = area.getX() + 160.0f;
    float cy = area.getY() + 150.0f;

    // 1. Outer Ring with 12 Note Nodes (R = 120)
    float outerRadius = 118.0f;
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

        float nodeW = isDetected ? 30.0f : 24.0f;
        auto nodeRect = juce::Rectangle<float>(nx - nodeW * 0.5f, ny - nodeW * 0.5f, nodeW, nodeW);

        if (isDetected)
        {
            g.setColour(juce::Colour(0xff00f5ff).withAlpha(0.45f));
            g.fillEllipse(nx - 22.0f, ny - 22.0f, 44.0f, 44.0f);

            g.setColour(juce::Colour(0xff00f5ff));
            g.fillEllipse(nodeRect);
            g.setColour(juce::Colours::white);
            g.drawEllipse(nodeRect, 1.8f);

            g.setFont(juce::Font(11.0f, juce::Font::bold));
            g.setColour(juce::Colour(0xff05030a));
            g.drawText(VeviAudio::NoteNames[i], nodeRect, juce::Justification::centred);
        }
        else if (inScale)
        {
            g.setColour(juce::Colour(0xff180b2c));
            g.fillEllipse(nodeRect);
            g.setColour(juce::Colour(0xffc084fc));
            g.drawEllipse(nodeRect, 1.4f);

            g.setFont(juce::Font(10.0f, juce::Font::bold));
            g.setColour(juce::Colour(0xfff3e8ff));
            g.drawText(VeviAudio::NoteNames[i], nodeRect, juce::Justification::centred);
        }
        else
        {
            g.setColour(juce::Colour(0xff0a0712));
            g.fillEllipse(nodeRect);
            g.setColour(juce::Colour(0xff2d1f42));
            g.drawEllipse(nodeRect, 1.0f);

            g.setFont(juce::Font(9.0f, juce::Font::plain));
            g.setColour(juce::Colour(0xff6b5585));
            g.drawText(VeviAudio::NoteNames[i], nodeRect, juce::Justification::centred);
        }
    }

    // 2. Circular Segmented Cyan LED Ring
    float segInnerR = 80.0f;
    float segOuterR = 98.0f;
    int numSegments = 30;

    auto* rawSpeedParam = audioProcessor.apvts.getRawParameterValue(TPainSupremeAudioProcessor::ID_RETUNE_SPEED);
    float currentSpeed = (rawSpeedParam != nullptr) ? rawSpeedParam->load() : 10.0f;
    float snapNorm = juce::jlimit(0.0f, 1.0f, 1.0f - (currentSpeed / 80.0f));

    for (int s = 0; s < numSegments; ++s)
    {
        float sAng = (static_cast<float>(s) / static_cast<float>(numSegments)) * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
        float cosA = std::cos(sAng);
        float sinA = std::sin(sAng);

        bool isLit = (s <= static_cast<int>(numSegments * (isVoiced ? snapNorm : 0.65f)));

        juce::Colour segColor = isLit ? juce::Colour(0xff00f5ff) : juce::Colour(0xff082838);
        g.setColour(segColor);

        float p1x = cx + segInnerR * cosA;
        float p1y = cy + segInnerR * sinA;
        float p2x = cx + segOuterR * cosA;
        float p2y = cy + segOuterR * sinA;

        g.drawLine(p1x, p1y, p2x, p2y, 4.5f);

        if (isLit)
        {
            g.setColour(juce::Colours::white.withAlpha(0.6f));
            g.drawLine(p1x, p1y, p2x, p2y, 1.5f);
        }
    }

    g.setColour(juce::Colour(0xff0e1824));
    g.drawEllipse(cx - segOuterR - 2.0f, cy - segOuterR - 2.0f, (segOuterR + 2.0f) * 2.0f, (segOuterR + 2.0f) * 2.0f, 1.2f);
    g.drawEllipse(cx - segInnerR + 2.0f, cy - segInnerR + 2.0f, (segInnerR - 2.0f) * 2.0f, (segInnerR - 2.0f) * 2.0f, 1.2f);

    // 3. Central Machined Titan Dial / Encoder (R = 54)
    float dialR = 54.0f;
    juce::ColourGradient dialGrad(
        juce::Colour(0xff231d30), cx - dialR * 0.4f, cy - dialR * 0.5f,
        juce::Colour(0xff0b0812), cx + dialR * 0.5f, cy + dialR * 0.6f, true);
    g.setGradientFill(dialGrad);
    g.fillEllipse(cx - dialR, cy - dialR, dialR * 2.0f, dialR * 2.0f);

    for (float rLine = 12.0f; rLine < dialR; rLine += 8.0f)
    {
        g.setColour(juce::Colour(0xffffffff).withAlpha(0.04f));
        g.drawEllipse(cx - rLine, cy - rLine, rLine * 2.0f, rLine * 2.0f, 0.8f);
    }

    g.setColour(juce::Colour(0xffc084fc).withAlpha(0.85f));
    g.drawEllipse(cx - dialR, cy - dialR, dialR * 2.0f, dialR * 2.0f, 1.4f);

    // Vertical Luminous Magenta Indicator Needle on Dial
    g.setColour(juce::Colour(0xffe879f9));
    g.drawLine(cx, cy - 8.0f, cx, cy - dialR + 6.0f, 3.2f);
    g.setColour(juce::Colours::white);
    g.drawLine(cx, cy - 8.0f, cx, cy - dialR + 6.0f, 1.4f);

    // 4. Digital Vertical LED Bar Meters to the right of wheel
    auto meterArea = juce::Rectangle<float>(cx + outerRadius + 24.0f, cy - 84.0f, 64.0f, 168.0f);
    drawVerticalDigitalBarMeters(g, meterArea);
}

void TPainSupremeAudioProcessorEditor::drawVerticalDigitalBarMeters(juce::Graphics& g, juce::Rectangle<float> area)
{
    float colW = 12.0f;
    float lColX = area.getX() + 6.0f;
    float rColX = area.getX() + 26.0f;
    float meterH = area.getHeight() - 24.0f;

    g.setColour(juce::Colour(0xff06040a));
    g.fillRoundedRectangle(lColX, area.getY(), colW, meterH, 2.0f);
    g.fillRoundedRectangle(rColX, area.getY(), colW, meterH, 2.0f);

    int numBars = 16;
    float barH = (meterH - 4.0f) / static_cast<float>(numBars);

    float inNorm = juce::jlimit(0.0f, 1.0f, (currentInputMeterDb + 50.0f) / 54.0f);
    float outNorm = juce::jlimit(0.0f, 1.0f, (currentOutputMeterDb + 50.0f) / 54.0f);

    for (int b = 0; b < numBars; ++b)
    {
        float barNorm = static_cast<float>(b) / static_cast<float>(numBars);
        float barY = area.getY() + meterH - (b + 1) * barH;

        juce::Colour c = (b > 12) ? juce::Colour(0xffffffff) :
                         (b > 8)  ? juce::Colour(0xff00f5ff) : juce::Colour(0xff0284c7);

        if (inNorm >= barNorm)
        {
            g.setColour(c);
            g.fillRect(lColX + 1.5f, barY + 1.0f, colW - 3.0f, barH - 1.5f);
        }
        else
        {
            g.setColour(juce::Colour(0xff111827));
            g.fillRect(lColX + 1.5f, barY + 1.0f, colW - 3.0f, barH - 1.5f);
        }

        if (outNorm >= barNorm)
        {
            g.setColour(c);
            g.fillRect(rColX + 1.5f, barY + 1.0f, colW - 3.0f, barH - 1.5f);
        }
        else
        {
            g.setColour(juce::Colour(0xff111827));
            g.fillRect(rColX + 1.5f, barY + 1.0f, colW - 3.0f, barH - 1.5f);
        }
    }

    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff94a3b8));
    g.drawText("VU", (int)lColX - 2, (int)(area.getY() + meterH + 4), (int)colW + 4, 12, juce::Justification::centred);
    g.drawText("Pkts", (int)rColX - 4, (int)(area.getY() + meterH + 4), (int)colW + 8, 12, juce::Justification::centred);
}

//==============================================================================
// 24K Brushed Gold Master Plate
//==============================================================================
void TPainSupremeAudioProcessorEditor::draw24KGoldMasterPlate(juce::Graphics& g, juce::Rectangle<float> area)
{
    g.setColour(juce::Colour(0x88000000));
    g.fillRoundedRectangle(area.translated(0.0f, 4.0f), 6.0f);

    juce::ColourGradient goldPlate(
        juce::Colour(0xffd4af37), area.getX(), area.getY(),
        juce::Colour(0xff8c6214), area.getRight(), area.getBottom(), false);
    goldPlate.addColour(0.25, juce::Colour(0xfffff099));
    goldPlate.addColour(0.65, juce::Colour(0xffb8860b));
    g.setGradientFill(goldPlate);
    g.fillRoundedRectangle(area, 6.0f);

    g.setColour(juce::Colour(0xffffffff).withAlpha(0.06f));
    for (float xLine = area.getX() + 4.0f; xLine < area.getRight() - 4.0f; xLine += 3.0f)
        g.drawVerticalLine((int)xLine, area.getY() + 4.0f, area.getBottom() - 4.0f);

    g.setColour(juce::Colour(0xffffe680));
    g.drawRoundedRectangle(area, 6.0f, 1.4f);

    // 4 Corner Gold Hex Screws
    float screwOffset = 14.0f;
    juce::Point<float> corners[] = {
        { area.getX() + screwOffset, area.getY() + screwOffset },
        { area.getRight() - screwOffset, area.getY() + screwOffset },
        { area.getX() + screwOffset, area.getBottom() - screwOffset },
        { area.getRight() - screwOffset, area.getBottom() - screwOffset }
    };

    for (const auto& pt : corners)
    {
        g.setColour(juce::Colour(0xff573a06));
        g.fillEllipse(pt.x - 5.5f, pt.y - 5.5f, 11.0f, 11.0f);
        juce::ColourGradient screwHead(juce::Colour(0xfffff2a3), pt.x - 4.0f, pt.y - 4.0f, juce::Colour(0xff8a6012), pt.x + 4.0f, pt.y + 4.0f, false);
        g.setGradientFill(screwHead);
        g.fillEllipse(pt.x - 4.5f, pt.y - 4.5f, 9.0f, 9.0f);

        g.setColour(juce::Colour(0xff3d2703));
        g.drawLine(pt.x - 2.5f, pt.y, pt.x + 2.5f, pt.y, 1.0f);
        g.drawLine(pt.x, pt.y - 2.5f, pt.x, pt.y + 2.5f, 1.0f);
    }

    // Vertical Recessed Slot
    float slotW = 7.0f;
    float slotX = area.getCentreX() - slotW * 0.5f;
    float slotY = area.getY() + 46.0f;
    float slotH = 340.0f;

    auto slotRect = juce::Rectangle<float>(slotX, slotY, slotW, slotH);
    g.setColour(juce::Colour(0xff090604));
    g.fillRoundedRectangle(slotRect, 3.5f);
    g.setColour(juce::Colour(0xff573a06));
    g.drawRoundedRectangle(slotRect, 3.5f, 1.2f);

    // Decibel markings
    struct DBMark { float relY; const char* txt; };
    DBMark marks[] = {
        { 0.05f, "0" },
        { 0.15f, "-5" },
        { 0.26f, "-10" },
        { 0.38f, "-15" },
        { 0.50f, "-20" },
        { 0.64f, "-30" },
        { 0.78f, "-40" },
        { 0.88f, "-50" },
        { 0.98f, "-60" }
    };

    g.setFont(juce::Font(9.5f, juce::Font::bold));
    for (const auto& m : marks)
    {
        float my = slotY + m.relY * slotH;
        g.setColour(juce::Colour(0xff452d06));
        g.drawLine(slotX - 12.0f, my, slotX - 3.0f, my, 1.2f);
        g.drawLine(slotX + slotW + 3.0f, my, slotX + slotW + 12.0f, my, 1.2f);
        g.drawText(m.txt, (int)(slotX - 38.0f), (int)(my - 7.0f), 24, 14, juce::Justification::right);
    }

    // Serigraphy at bottom of plate: "3.6 MASTER LEVEL"
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff4a3106));
    g.drawText("3.6", (int)area.getX(), (int)(area.getBottom() - 44.0f), (int)area.getWidth(), 16, juce::Justification::centred);
    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.drawText("MASTER", (int)area.getX(), (int)(area.getBottom() - 30.0f), (int)area.getWidth(), 14, juce::Justification::centred);
    g.drawText("LEVEL", (int)area.getX(), (int)(area.getBottom() - 18.0f), (int)area.getWidth(), 14, juce::Justification::centred);
}

//==============================================================================
// Main Paint Method
//==============================================================================
void TPainSupremeAudioProcessorEditor::paint(juce::Graphics& g)
{
    int w = getWidth();
    int h = getHeight();

    // 1. Brushed Anodized Obsidian Black Metal Chassis
    juce::ColourGradient chassisGrad(
        juce::Colour(0xff120f1a), 0.0f, 0.0f,
        juce::Colour(0xff09070e), 0.0f, (float)h, false);
    g.setGradientFill(chassisGrad);
    g.fillAll();

    // Subtle horizontal brushed texture
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.018f));
    for (int y = 0; y < h; y += 4)
        g.drawHorizontalLine(y, 44.0f, (float)w - 44.0f);

    g.setColour(juce::Colour(0xffffffff).withAlpha(0.08f));
    g.drawHorizontalLine(1, 44.0f, (float)w - 44.0f);
    g.setColour(juce::Colour(0xff000000));
    g.drawHorizontalLine(h - 1, 44.0f, (float)w - 44.0f);

    // 2. Hardware 19" Rack Ears with Screws and Neon LED Bars
    drawHardwareRackEars(g, w, h);

    // 3. Bottom-Left 1/4" Golden Jack
    drawQuarterInchJack(g, 90.0f, 574.0f, 15.0f);

    // 4. Header Branding Serigraphy
    // Left: SUPREME TUNER (electric cyan) & Real Time v3.6 (neon magenta)
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00f5ff));
    g.drawText("SUPREME TUNER", 74, 24, 300, 28, juce::Justification::left);

    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffe879f9));
    g.drawText("Real Time v3.6", 74, 52, 200, 20, juce::Justification::left);

    // Right: ULTRA-PREMIUM
    g.setFont(juce::Font(12.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffcbd5e1));
    g.drawText("ULTRA-PREMIUM", w - 210, 36, 150, 18, juce::Justification::right);

    // 5. Left Bay: Dual Vintage Analog VU Meters
    auto vuBay = juce::Rectangle<float>(74.0f, 96.0f, 350.0f, 246.0f);
    drawDualAnalogVUMeters(g, vuBay);

    // 6. Center Stage: Chromatic Segmented Pitch Wheel & Digital Bar Meters
    auto wheelBay = juce::Rectangle<float>(444.0f, 76.0f, 376.0f, 300.0f);
    drawSegmentedPitchWheel(g, wheelBay);

    // 7. Right Bay: 24K Brushed Gold Master Plate
    auto goldBay = juce::Rectangle<float>((float)w - 230.0f, 84.0f, 166.0f, 486.0f);
    draw24KGoldMasterPlate(g, goldBay);
}

//==============================================================================
// Resized Layout: Perfect 19" Hardware Chassis Alignment
//==============================================================================
void TPainSupremeAudioProcessorEditor::resized()
{
    int w = getWidth();

    // Top Selectors (Neatly integrated in top area)
    presetSelector.setBounds(440, 34, 210, 26);
    rootSelector.setBounds(660, 34, 100, 26);
    scaleSelector.setBounds(770, 34, 130, 26);

    // 4 Bottom Studio Knobs (SPEED, AMOUNT, SENS, MIX)
    int knobY = 412;
    int knobW = 104;
    int knobH = 104;
    int knobGap = 132;
    int startKnobX = 186;

    // 1. SPEED
    retuneSpeedSlider.setBounds(startKnobX, knobY, knobW, knobH);
    speedLabel.setBounds(startKnobX - 10, knobY + knobH + 12, knobW + 20, 16);

    // 2. AMOUNT
    centeringSlider.setBounds(startKnobX + knobGap, knobY, knobW, knobH);
    centerLabel.setBounds(startKnobX + knobGap - 10, knobY + knobH + 12, knobW + 20, 16);

    // 3. SENS
    gateThreshSlider.setBounds(startKnobX + knobGap * 2, knobY, knobW, knobH);
    gateThrLabel.setBounds(startKnobX + knobGap * 2 - 10, knobY + knobH + 12, knobW + 20, 16);

    // 4. MIX
    mixSlider.setBounds(startKnobX + knobGap * 3, knobY, knobW, knobH);
    mixLabel.setBounds(startKnobX + knobGap * 3 - 10, knobY + knobH + 12, knobW + 20, 16);

    // 24K Gold Master Fader (outputSlider)
    float plateX = (float)w - 230.0f;
    outputSlider.setBounds(static_cast<int>(plateX + 18.0f), 130, 130, 340);
    outLabel.setBounds(0, 0, 0, 0);

    // Hide secondary controls from main hardware faceplate
    transitionSlider.setBounds(0, 0, 0, 0);
    transSpeedLabel.setBounds(0, 0, 0, 0);
    variationSlider.setBounds(0, 0, 0, 0);
    varLabel.setBounds(0, 0, 0, 0);

    transientsSlider.setBounds(0, 0, 0, 0);
    transLabel.setBounds(0, 0, 0, 0);
    compressionSlider.setBounds(0, 0, 0, 0);
    compLabel.setBounds(0, 0, 0, 0);

    lowCutSlider.setBounds(0, 0, 0, 0);
    lowCutLabel.setBounds(0, 0, 0, 0);
    bodySlider.setBounds(0, 0, 0, 0);
    bodyLabel.setBounds(0, 0, 0, 0);
    airSlider.setBounds(0, 0, 0, 0);
    airLabel.setBounds(0, 0, 0, 0);

    reverbMixSlider.setBounds(0, 0, 0, 0);
    revMixLabel.setBounds(0, 0, 0, 0);
    reverbSizeSlider.setBounds(0, 0, 0, 0);
    revSizeLabel.setBounds(0, 0, 0, 0);
    delayMixSlider.setBounds(0, 0, 0, 0);
    dlyMixLabel.setBounds(0, 0, 0, 0);
    delayTimeSlider.setBounds(0, 0, 0, 0);
    dlyTimeLabel.setBounds(0, 0, 0, 0);
    delayFeedbackSlider.setBounds(0, 0, 0, 0);
    dlyFbLabel.setBounds(0, 0, 0, 0);

    inputGainSlider.setBounds(0, 0, 0, 0);
    inputGainLabel.setBounds(0, 0, 0, 0);
    stereoWidthSlider.setBounds(0, 0, 0, 0);
    stereoWidthLabel.setBounds(0, 0, 0, 0);

    gateAttackSlider.setBounds(0, 0, 0, 0);
    gateAtkLabel.setBounds(0, 0, 0, 0);
    gateReleaseSlider.setBounds(0, 0, 0, 0);
    gateRelLabel.setBounds(0, 0, 0, 0);
}
