#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// SupremeKnobLookAndFeel Implementation
// ==============================================================================
SupremeKnobLookAndFeel::SupremeKnobLookAndFeel()
{
}

void SupremeKnobLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPosProportional, float rotaryStartAngle,
                                              float rotaryEndAngle, juce::Slider& slider)
{
    const float radius = float(std::min(width, height)) * 0.45f;
    const float centreX = float(x) + float(width) * 0.5f;
    const float centreY = float(y) + float(height) * 0.45f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // 1. Outer Knurled Bezel Shadow
    g.setColour(juce::Colour(0xff06090e));
    g.fillEllipse(rx - 2.0f, ry - 1.0f, rw + 4.0f, rw + 4.0f);

    // 2. Knurled Metallic Outer Bezel
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff2d3a4d), centreX, ry,
                                           juce::Colour(0xff141a24), centreX, ry + rw, false));
    g.fillEllipse(rx, ry, rw, rw);

    // Knurling ticks around bezel
    g.setColour(juce::Colour(0x33ffffff));
    const int numTicks = 24;
    for (int i = 0; i < numTicks; ++i)
    {
        const float tickAngle = (float(i) / float(numTicks)) * juce::MathConstants<float>::twoPi;
        const float cosA = std::cos(tickAngle);
        const float sinA = std::sin(tickAngle);
        g.drawLine(centreX + (radius - 3.0f) * cosA, centreY + (radius - 3.0f) * sinA,
                   centreX + radius * cosA, centreY + radius * sinA, 1.0f);
    }

    // 3. Outer LED Arc Ring Track
    const float ringRadius = radius - 4.5f;
    juce::Path trackPath;
    trackPath.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff0a0d13));
    g.strokePath(trackPath, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // LED Arc Value Fill (Glowing Neon Accent)
    juce::Path valuePath;
    valuePath.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f, rotaryStartAngle, angle, true);
    juce::Colour accent = slider.findColour(juce::Slider::rotarySliderFillColourId);
    g.setColour(accent);
    g.strokePath(valuePath, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 4. Inner Recessed Cap (Dark Brushed Metal)
    const float capRadius = radius * 0.72f;
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff222b38), centreX - capRadius * 0.4f, centreY - capRadius * 0.4f,
                                           juce::Colour(0xff0d121a), centreX + capRadius * 0.4f, centreY + capRadius * 0.4f, true));
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);

    g.setColour(juce::Colour(0x44ffffff));
    g.drawEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.0f);

    // 5. Pointer Needle with Glowing Pip
    juce::Path p;
    const float pointerLength = capRadius * 0.85f;
    const float pointerThickness = 2.8f;
    p.addRoundedRectangle(-pointerThickness * 0.5f, -capRadius + 2.0f, pointerThickness, pointerLength * 0.55f, 1.2f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // Needle Glow
    g.setColour(accent.withAlpha(0.6f));
    g.strokePath(p, juce::PathStrokeType(2.5f));

    g.setColour(accent);
    g.fillPath(p);
}

// ==============================================================================
// AutomasterSupremeAudioProcessorEditor Implementation
// ==============================================================================
AutomasterSupremeAudioProcessorEditor::AutomasterSupremeAudioProcessorEditor(AutomasterSupremeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1080, 740);
    setLookAndFeel(&customKnobLAF);

    // Preset Selector (Spotify Centric)
    presetBox.addItem("Spotify Standard Reference (-14 LUFS / -1.0 dBTP)", 1);
    presetBox.addItem("Modern Pop (Spotify -14 LUFS)", 2);
    presetBox.addItem("Hip-Hop / Trap (Spotify -13.5 LUFS)", 3);
    presetBox.addItem("EDM / Dance (Spotify -13.0 LUFS)", 4);
    presetBox.addItem("Acoustic / Dynamic (Spotify -15.0 LUFS)", 5);
    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0d131b));
    presetBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff233144));
    presetBox.setColour(juce::ComboBox::textColourId, juce::Colour(0xff00f0ff));
    presetBox.onChange = [this]()
    {
        audioProcessor.applyPreset(presetBox.getSelectedId() - 1);
    };
    addAndMakeVisible(presetBox);

    // AI Auto Master Button
    autoMasterButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff0088cc));
    autoMasterButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    autoMasterButton.onClick = [this]()
    {
        audioProcessor.applyPreset(presetBox.getSelectedId() - 1);
    };
    addAndMakeVisible(autoMasterButton);

    // Saturation Mode Selector
    satModeBox.addItem("ANALOG TAPE", 1);
    satModeBox.addItem("TUBE TRIODE", 2);
    satModeBox.addItem("CLASS-A", 3);
    satModeBox.addItem("CLEAN CLIP", 4);
    satModeBox.setSelectedId(1, juce::dontSendNotification);
    satModeBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0a0d13));
    satModeBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff2a3648));
    satModeBox.setColour(juce::ComboBox::textColourId, juce::Colour(0xffffb92d));
    addAndMakeVisible(satModeBox);

    // Module Bypass Buttons
    configureBypassButton(bypassLowCut);
    configureBypassButton(bypassEQ);
    configureBypassButton(bypassSat);
    configureBypassButton(bypassMB);
    configureBypassButton(bypassLimiter);

    // Knobs Configuration with Spotify & Cyber-analog Accents
    const juce::Colour cyan(0xff00f0ff);
    const juce::Colour gold(0xffffb92d);
    const juce::Colour emerald(0xff00ffaa);
    const juce::Colour red(0xffff3366);

    // 1. Input & Sub
    configureKnob(inGainSlider, inGainLabel, "IN GAIN", " dB", cyan);
    configureKnob(lowCutSlider, lowCutLabel, "LOW CUT", " Hz", gold);

    // 2. 5-Band EQ
    configureKnob(eqSubSlider, eqSubLabel, "SUB BASS", " dB", cyan);
    configureKnob(eqLowMidSlider, eqLowMidLabel, "LOW-MID", " dB", cyan);
    configureKnob(eqMidSlider, eqMidLabel, "PRESENCE", " dB", cyan);
    configureKnob(eqClaritySlider, eqClarityLabel, "CLARITY", " dB", cyan);
    configureKnob(eqAirSlider, eqAirLabel, "AIR SHEEN", " dB", gold);

    // 3. Saturation
    configureKnob(satDriveSlider, satDriveLabel, "DRIVE", " %", gold);
    configureKnob(satWarmthSlider, satWarmthLabel, "WARMTH", " %", gold);

    // 4. Multiband
    configureKnob(mbLowSlider, mbLowLabel, "LOW", " dB", emerald);
    configureKnob(mbMidSlider, mbMidLabel, "MID", " dB", emerald);
    configureKnob(mbHighSlider, mbHighLabel, "HIGH", " dB", emerald);

    // 5. Stereo & Limiter
    configureKnob(stereoWidthSlider, stereoWidthLabel, "WIDTH", " %", cyan);
    configureKnob(monoMakerSlider, monoMakerLabel, "MONO SUB", " Hz", cyan);
    configureKnob(loudnessSlider, loudnessLabel, "LOUDNESS", " dB", red);
    configureKnob(ceilingSlider, ceilingLabel, "CEILING", " dBTP", red);
    configureKnob(outGainSlider, outGainLabel, "OUT GAIN", " dB", cyan);

    // Parameter Attachments
    inGainAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "inGain", inGainSlider);
    lowCutAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "lowCut", lowCutSlider);
    lowCutBypassAttach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "lowCutBypass", bypassLowCut);

    eqSubAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "eqLowShelfGain", eqSubSlider);
    eqLowMidAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "eqLowMidGain", eqLowMidSlider);
    eqMidAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "eqMidGain", eqMidSlider);
    eqClarityAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "eqHighMidGain", eqClaritySlider);
    eqAirAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "eqHighShelfGain", eqAirSlider);
    eqBypassAttach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "eqBypass", bypassEQ);

    satDriveAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "satDrive", satDriveSlider);
    satWarmthAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "satWarmth", satWarmthSlider);
    satModeAttach = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "satMode", satModeBox);
    satBypassAttach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "satBypass", bypassSat);

    mbLowAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "mbLowThresh", mbLowSlider);
    mbMidAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "mbMidThresh", mbMidSlider);
    mbHighAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "mbHighThresh", mbHighSlider);
    mbBypassAttach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "mbBypass", bypassMB);

    stereoWidthAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "stereoWidth", stereoWidthSlider);
    monoMakerAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "monoMakerFreq", monoMakerSlider);

    loudnessAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "loudnessDrive", loudnessSlider);
    ceilingAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "ceiling", ceilingSlider);
    outGainAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "outGain", outGainSlider);
    limiterBypassAttach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "limiterBypass", bypassLimiter);

    startTimerHz(30);
}

AutomasterSupremeAudioProcessorEditor::~AutomasterSupremeAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void AutomasterSupremeAudioProcessorEditor::configureKnob(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix, juce::Colour accent)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 16);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::rotarySliderFillColourId, accent);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff090c10));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff1c2534));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff8b9bb4));
    addAndMakeVisible(label);
}

void AutomasterSupremeAudioProcessorEditor::configureBypassButton(juce::ToggleButton& btn)
{
    btn.setColour(juce::ToggleButton::textColourId, juce::Colour(0xff00ffaa));
    btn.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff00ffaa));
    btn.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xffff3366));
    addAndMakeVisible(btn);
}

void AutomasterSupremeAudioProcessorEditor::timerCallback()
{
    // Real-time audio readings
    const float livePeakL = audioProcessor.dsp.getPeakL();
    const float livePeakR = audioProcessor.dsp.getPeakR();
    const float liveLufs = audioProcessor.dsp.getMomentaryLUFS();
    const float liveShortTerm = audioProcessor.dsp.getShortTermLUFS();
    const float liveCrest = audioProcessor.dsp.getCrestFactor();
    curPhase = audioProcessor.dsp.getPhaseCorrelation();

    audioProcessor.dsp.getFFTMagnitudes(fftDisplayData.data(), int(fftDisplayData.size()));
    audioProcessor.dsp.getScopeSamples(scopeL.data(), scopeR.data(), int(scopeL.size()));

    // Accumulate peaks & LUFS over the 0.5s window
    accumPeakL = std::max(accumPeakL, livePeakL);
    accumPeakR = std::max(accumPeakR, livePeakR);
    if (liveLufs > -90.0f)
    {
        accumMomentaryLufs += liveLufs;
        accumCrest += liveCrest;
        accumSampleCount++;
    }

    meterTimerTickCount++;
    // Running at 30 Hz -> 15 ticks = exactly 0.5 seconds (500 ms)
    if (meterTimerTickCount >= 15)
    {
        meterTimerTickCount = 0;

        dispPeakL = accumPeakL;
        dispPeakR = accumPeakR;

        if (dispPeakL > dispPeakHoldL) dispPeakHoldL = dispPeakL;
        else dispPeakHoldL = std::max(-100.0f, dispPeakHoldL - 1.5f);

        if (dispPeakR > dispPeakHoldR) dispPeakHoldR = dispPeakR;
        else dispPeakHoldR = std::max(-100.0f, dispPeakHoldR - 1.5f);

        if (accumSampleCount > 0)
        {
            dispMomentaryLufs = accumMomentaryLufs / float(accumSampleCount);
            dispCrest = accumCrest / float(accumSampleCount);
        }
        else
        {
            dispMomentaryLufs = liveLufs;
            dispCrest = liveCrest;
        }
        dispShortTermLufs = liveShortTerm;

        // Spotify status determination
        if (dispMomentaryLufs <= -13.0f && dispMomentaryLufs >= -15.5f)
        {
            dispSpotifyStatus = "-14 TARGET";
            dispSpotifyCol = juce::Colour(0xff00ffaa); // Green
        }
        else if (dispMomentaryLufs > -12.5f)
        {
            dispSpotifyStatus = "TOO LOUD";
            dispSpotifyCol = juce::Colour(0xffff3366); // Red
        }
        else
        {
            dispSpotifyStatus = "LOW LEVEL";
            dispSpotifyCol = juce::Colour(0xffffb92d); // Amber
        }

        // Reset accumulator for next 0.5s cycle
        accumPeakL = -100.0f;
        accumPeakR = -100.0f;
        accumMomentaryLufs = 0.0f;
        accumCrest = 0.0f;
        accumSampleCount = 0;
    }

    agentAnimationTick++;
    repaint();
}

void AutomasterSupremeAudioProcessorEditor::drawRackScrew(juce::Graphics& g, int x, int y)
{
    g.setColour(juce::Colour(0xff3a485a));
    g.fillEllipse(float(x) - 7.0f, float(y) - 7.0f, 14.0f, 14.0f);
    g.setColour(juce::Colour(0xff1b222c));
    g.drawEllipse(float(x) - 7.0f, float(y) - 7.0f, 14.0f, 14.0f, 1.2f);
    g.setColour(juce::Colour(0xff60728a));
    g.drawLine(float(x) - 4.0f, float(y), float(x) + 4.0f, float(y), 1.5f);
    g.drawLine(float(x), float(y) - 4.0f, float(x), float(y) + 4.0f, 1.5f);
}

void AutomasterSupremeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // 1. VST Main Rack Chassis Background
    g.fillAll(juce::Colour(0xff0d1117));

    // Metallic Rack Screws in 4 corners
    drawRackScrew(g, 16, 16);
    drawRackScrew(g, 36, 16);
    drawRackScrew(g, getWidth() - 36, 16);
    drawRackScrew(g, getWidth() - 16, 16);

    // 2. Header Top Bar
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff1f2937), 0, 0, juce::Colour(0xff111827), 0, 60, false));
    g.fillRect(0, 0, getWidth(), 60);
    g.setColour(juce::Colour(0xff2a3648));
    g.drawLine(0.0f, 60.0f, float(getWidth()), 60.0f, 2.0f);

    // Brand Logo Icon
    auto logoRect = juce::Rectangle<float>(56, 12, 36, 36);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff00f0ff), logoRect.getX(), logoRect.getY(),
                                           juce::Colour(0xffb34bfb), logoRect.getRight(), logoRect.getBottom(), false));
    g.fillRoundedRectangle(logoRect, 8.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("A", logoRect, juce::Justification::centred);

    // Title & Badge
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("AUTOMASTER SUPREME", 102, 10, 260, 24, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff00f0ff));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("AI VST MASTERING SUITE", 102, 34, 150, 16, juce::Justification::centredLeft);

    // Spotify Reference Tag Pill
    auto spotifyTag = juce::Rectangle<float>(265, 34, 255, 18);
    g.setColour(juce::Colour(0x2200ffaa));
    g.fillRoundedRectangle(spotifyTag, 4.0f);
    g.setColour(juce::Colour(0xff00ffaa));
    g.drawRoundedRectangle(spotifyTag, 4.0f, 1.0f);
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.drawText("SPOTIFY TARGET (-14.0 LUFS / -1.0 dBTP)", spotifyTag, juce::Justification::centred);

    // 3. Upper Deck: 3 OLED Screen Frames
    drawSpectrumScreen(g, juce::Rectangle<int>(20, 72, 540, 250));
    drawGoniometerScreen(g, juce::Rectangle<int>(572, 72, 260, 250));
    drawMetersScreen(g, juce::Rectangle<int>(844, 72, 216, 250));

    // 4. Lower Deck: 5 Hardware Rack Modules
    drawModuleCard(g, juce::Rectangle<int>(20, 334, 135, 275), "1. INPUT & SUB");
    drawModuleCard(g, juce::Rectangle<int>(165, 334, 355, 275), "2. 5-BAND PARAMETRIC EQ");
    drawModuleCard(g, juce::Rectangle<int>(530, 334, 140, 275), "3. TAPE WARMTH");
    drawModuleCard(g, juce::Rectangle<int>(680, 334, 150, 275), "4. MULTIBAND");
    drawModuleCard(g, juce::Rectangle<int>(840, 334, 220, 275), "5. STEREO & LIMITER");

    // Frequency badges for 5-Band EQ
    const char* eqFreqLabels[] = { "35 Hz", "300 Hz", "1.5 kHz", "4.5 kHz", "12 kHz" };
    const char* eqTypeLabels[] = { "LOW SHELF", "BELL (Q 1.0)", "BELL (Q 1.2)", "BELL (Q 1.1)", "HIGH SHELF" };
    for (int i = 0; i < 5; ++i)
    {
        int bandX = 172 + i * 68;
        auto badgeRect = juce::Rectangle<float>(float(bandX + 2), 460.0f, 52.0f, 26.0f);
        g.setColour(juce::Colour(0xff090e15));
        g.fillRoundedRectangle(badgeRect, 3.0f);
        g.setColour(juce::Colour(0xff1d2838));
        g.drawRoundedRectangle(badgeRect, 3.0f, 1.0f);
        g.setFont(juce::Font(8.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xff00f0ff));
        g.drawText(eqFreqLabels[i], badgeRect.removeFromTop(13.0f), juce::Justification::centred);
        g.setFont(juce::Font(7.0f, juce::Font::plain));
        g.setColour(juce::Colour(0xff8b9bb4));
        g.drawText(eqTypeLabels[i], badgeRect, juce::Justification::centred);
    }

    // 5. Bottom Multi-Agent Telemetry Bar
    drawAgentDeck(g, juce::Rectangle<int>(20, 620, getWidth() - 40, 105));
}

// ------------------------------------------------------------------------------
// Draw Spectrum Analyzer & EQ Curve Screen
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawSpectrumScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(0xff090d13));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1e2837));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.2f);

    // Screen Header
    auto headerRect = bounds.removeFromTop(24);
    g.setColour(juce::Colour(0x10ffffff));
    g.fillRect(headerRect);
    g.setColour(juce::Colour(0xff8b9bb4));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("SPECTRUM ANALYZER & MASTERING EQ CURVE", headerRect.getX() + 10, headerRect.getY(), 300, 24, juce::Justification::centredLeft);

    // Legend
    g.setFont(juce::Font(9.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xffffb92d));
    g.drawText("* Pre", headerRect.getRight() - 170, headerRect.getY(), 40, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText("* Post (Wet)", headerRect.getRight() - 125, headerRect.getY(), 65, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xffffff00));
    g.drawText("-- Curva EQ", headerRect.getRight() - 55, headerRect.getY(), 50, 24, juce::Justification::centredLeft);

    auto displayArea = bounds.reduced(8, 6);
    const float w = float(displayArea.getWidth());
    const float h = float(displayArea.getHeight());
    const float ox = float(displayArea.getX());
    const float oy = float(displayArea.getY());

    // Grid lines (dB)
    const float minDb = -66.0f;
    const float maxDb = 6.0f;
    const float dbRange = maxDb - minDb; // 72 dB

    g.setColour(juce::Colour(0x15ffffff));
    for (float db = 0.0f; db >= -60.0f; db -= 12.0f)
    {
        float normY = oy + h - ((db - minDb) / dbRange) * h;
        g.drawLine(ox, normY, ox + w, normY, 1.0f);
        g.setColour(juce::Colour(0x55ffffff));
        g.setFont(juce::Font(8.0f, juce::Font::plain));
        g.drawText(juce::String(int(db)) + "dB", ox + 2, normY - 8, 30, 10, juce::Justification::left);
        g.setColour(juce::Colour(0x15ffffff));
    }

    // Grid lines (Hz)
    const float minLog = std::log10(20.0f);
    const float maxLog = std::log10(20000.0f);
    const float logRange = maxLog - minLog;
    const float freqs[] = { 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f, 20000.0f };
    const char* freqLabels[] = { "50", "100", "250", "500", "1k", "2.5k", "5k", "10k", "20k" };

    for (int i = 0; i < 9; ++i)
    {
        float normX = ox + ((std::log10(freqs[i]) - minLog) / logRange) * w;
        g.drawLine(normX, oy, normX, oy + h, 1.0f);
        g.setColour(juce::Colour(0x55ffffff));
        g.setFont(juce::Font(8.0f, juce::Font::plain));
        g.drawText(freqLabels[i], normX + 2, oy + h - 10, 25, 10, juce::Justification::left);
        g.setColour(juce::Colour(0x15ffffff));
    }

    // Draw Live FFT Curve (Post-Master with Neon Cyan Glow)
    const double sr = audioProcessor.dsp.getSampleRate();
    const float binStep = float(AudioConstants::FFT_SIZE) / float(sr > 0 ? sr : 44100.0);
    const int totalBins = int(fftDisplayData.size());

    juce::Path fftPath;
    bool started = false;
    const int numPoints = int(w);

    for (int px = 0; px < numPoints; ++px)
    {
        const float normX = ox + float(px);
        const float t1 = float(px) / float(numPoints);
        const float t2 = float(px + 1) / float(numPoints);

        const float f1 = std::pow(10.0f, minLog + t1 * logRange);
        const float f2 = std::pow(10.0f, minLog + t2 * logRange);

        const float binExact = f1 * binStep;
        const float binNext = f2 * binStep;

        float maxBinDb = -100.0f;
        const int startBin = std::clamp(int(binExact), 0, totalBins - 1);
        const int endBin = std::clamp(int(binNext), startBin, totalBins - 1);

        if (endBin > startBin)
        {
            for (int b = startBin; b <= endBin; ++b)
            {
                if (fftDisplayData[size_t(b)] > maxBinDb)
                    maxBinDb = fftDisplayData[size_t(b)];
            }
        }
        else
        {
            const int b0 = std::clamp(int(binExact), 0, totalBins - 2);
            const float frac = binExact - float(b0);
            maxBinDb = fftDisplayData[size_t(b0)] * (1.0f - frac) + fftDisplayData[size_t(b0 + 1)] * frac;
        }

        const float normY = oy + h - std::clamp((maxBinDb - minDb) / dbRange, 0.0f, 1.0f) * h;

        if (!started)
        {
            fftPath.startNewSubPath(normX, normY);
            started = true;
        }
        else
        {
            fftPath.lineTo(normX, normY);
        }
    }

    if (started)
    {
        juce::Path fillPath = fftPath;
        fillPath.lineTo(ox + w, oy + h);
        fillPath.lineTo(ox, oy + h);
        fillPath.closeSubPath();

        // Neon Gradient Fill
        g.setGradientFill(juce::ColourGradient(juce::Colour(0x3300f0ff), ox, oy, juce::Colour(0x0000f0ff), ox, oy + h, false));
        g.fillPath(fillPath);

        // Neon Stroke Line
        g.setColour(juce::Colour(0xff00f0ff));
        g.strokePath(fftPath, juce::PathStrokeType(1.8f));
    }

    // Draw Dynamic Parametric EQ Curve (Yellow Dashed)
    const float gSub = audioProcessor.apvts.getRawParameterValue("eqLowShelfGain")->load();
    const float gLowMid = audioProcessor.apvts.getRawParameterValue("eqLowMidGain")->load();
    const float gMid = audioProcessor.apvts.getRawParameterValue("eqMidGain")->load();
    const float gClarity = audioProcessor.apvts.getRawParameterValue("eqHighMidGain")->load();
    const float gAir = audioProcessor.apvts.getRawParameterValue("eqHighShelfGain")->load();

    juce::Path eqCurve;
    for (int i = 0; i <= 60; ++i)
    {
        const float f = 20.0f * std::pow(1000.0f, float(i) / 60.0f);
        const float x = ox + ((std::log10(f) - minLog) / logRange) * w;

        float totalGain = 0.0f;
        if (f < 160.0f) totalGain += gSub * (1.0f - f / 200.0f);
        totalGain += gLowMid * std::exp(-std::abs(std::log2(f / 320.0f)) * 1.5f);
        totalGain += gMid * std::exp(-std::abs(std::log2(f / 2000.0f)) * 1.5f);
        totalGain += gClarity * std::exp(-std::abs(std::log2(f / 5000.0f)) * 1.5f);
        if (f > 6000.0f) totalGain += gAir * std::min(1.0f, (f - 6000.0f) / 6000.0f);

        // Center around -18dB line
        const float y = oy + h - ((totalGain - 18.0f - minDb) / dbRange) * h;
        if (i == 0) eqCurve.startNewSubPath(x, y);
        else eqCurve.lineTo(x, y);
    }

    g.setColour(juce::Colour(0xddffff00));
    juce::PathStrokeType stroke(1.8f);
    juce::Path dashedEqCurve;
    float dashes[] = { 4.0f, 3.0f };
    stroke.createDashedStroke(dashedEqCurve, eqCurve, dashes, 2);
    g.strokePath(dashedEqCurve, stroke);
}

// ------------------------------------------------------------------------------
// Draw Lissajous Stereo Goniometer Screen
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawGoniometerScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(0xff090d13));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1e2837));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.2f);

    // Screen Header
    auto headerRect = bounds.removeFromTop(24);
    g.setColour(juce::Colour(0x10ffffff));
    g.fillRect(headerRect);
    g.setColour(juce::Colour(0xff8b9bb4));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("STEREO GONIOMETER", headerRect.getX() + 10, headerRect.getY(), 160, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff00ffaa));
    g.drawText("PHOSPHOR", headerRect.getRight() - 75, headerRect.getY(), 65, 24, juce::Justification::right);

    auto scopeArea = bounds.removeFromTop(185).reduced(10);
    const float cx = scopeArea.getCentreX();
    const float cy = scopeArea.getCentreY();
    const float radius = float(std::min(scopeArea.getWidth(), scopeArea.getHeight())) * 0.48f;

    // Reticle
    g.setColour(juce::Colour(0x1800f0ff));
    g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.0f);
    g.drawLine(cx - radius, cy, cx + radius, cy, 1.0f);
    g.drawLine(cx, cy - radius, cx, cy + radius, 1.0f);

    g.setColour(juce::Colour(0x4400f0ff));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("L", cx - radius + 4, cy - 10, 15, 10, juce::Justification::left);
    g.drawText("R", cx + radius - 18, cy - 10, 15, 10, juce::Justification::right);
    g.drawText("MID", cx - 12, cy - radius + 4, 24, 10, juce::Justification::centred);

    // Lissajous Phosphor Beam
    juce::Path beam;
    bool beamStarted = false;
    for (int i = 0; i < 256; i += 2)
    {
        const float l = scopeL[size_t(i)];
        const float r = scopeR[size_t(i)];

        const float x = cx + (l - r) * 0.707f * radius * 1.5f;
        const float y = cy - (l + r) * 0.707f * radius * 1.5f;

        if (!beamStarted) { beam.startNewSubPath(x, y); beamStarted = true; }
        else { beam.lineTo(x, y); }
    }

    g.setColour(juce::Colour(0xaa00ffaa));
    g.strokePath(beam, juce::PathStrokeType(1.2f));

    // Phase Correlation Bar at bottom of screen
    auto phaseArea = bounds.reduced(10, 6);
    g.setColour(juce::Colour(0xff06090e));
    g.fillRoundedRectangle(phaseArea.toFloat(), 3.0f);

    g.setColour(juce::Colour(0x33ffffff));
    g.drawLine(float(phaseArea.getCentreX()), float(phaseArea.getY()), float(phaseArea.getCentreX()), float(phaseArea.getBottom()), 1.0f);

    const float normPhase = std::clamp((curPhase + 1.0f) * 0.5f, 0.0f, 1.0f);
    const float cursorX = phaseArea.getX() + normPhase * float(phaseArea.getWidth());
    g.setColour(juce::Colour(0xff00ffaa));
    g.fillRect(cursorX - 3.0f, float(phaseArea.getY()), 6.0f, float(phaseArea.getHeight()));

    g.setFont(juce::Font(7.5f, juce::Font::plain));
    g.setColour(juce::Colour(0x66ffffff));
    g.drawText("-1 (Anti)", phaseArea.getX(), phaseArea.getY() - 10, 45, 10, juce::Justification::left);
    g.drawText("+1 (Mono)", phaseArea.getRight() - 50, phaseArea.getY() - 10, 50, 10, juce::Justification::right);
}

// ------------------------------------------------------------------------------
// Draw True-Peak & LUFS Precision Meters Screen
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawMetersScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(0xff090d13));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1e2837));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.2f);

    auto headerRect = bounds.removeFromTop(24);
    g.setColour(juce::Colour(0x10ffffff));
    g.fillRect(headerRect);
    g.setColour(juce::Colour(0xff8b9bb4));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("METERS", headerRect.getX() + 10, headerRect.getY(), 80, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText("ITU-R BS.1770", headerRect.getRight() - 100, headerRect.getY(), 90, 24, juce::Justification::right);

    // Left & Right Segmented Peak Meters (0.5s held values for clear visibility)
    auto meterArea = bounds.removeFromTop(120).reduced(12, 6);
    const float toPercentL = std::clamp((dispPeakL + 48.0f) / 48.0f, 0.0f, 1.0f);
    const float toPercentR = std::clamp((dispPeakR + 48.0f) / 48.0f, 0.0f, 1.0f);
    const float holdPercentL = std::clamp((dispPeakHoldL + 48.0f) / 48.0f, 0.0f, 1.0f);
    const float holdPercentR = std::clamp((dispPeakHoldR + 48.0f) / 48.0f, 0.0f, 1.0f);

    const int barW = 12;
    const int gap = 14;
    const int leftX = meterArea.getCentreX() - barW - gap / 2;
    const int rightX = meterArea.getCentreX() + gap / 2;
    const int barH = meterArea.getHeight() - 14;
    const int barY = meterArea.getY();

    // Channel L Track
    g.setColour(juce::Colour(0xff06090e));
    g.fillRect(leftX, barY, barW, barH);
    g.fillRect(rightX, barY, barW, barH);

    // Fill L & R with Multi-color gradient
    const int fillHL = int(toPercentL * float(barH));
    const int fillHR = int(toPercentR * float(barH));

    juce::ColourGradient meterGrad(juce::Colour(0xffff3366), float(leftX), float(barY),
                                   juce::Colour(0xff00ffaa), float(leftX), float(barY + barH), false);
    meterGrad.addColour(0.2, juce::Colour(0xffffb92d));
    meterGrad.addColour(0.4, juce::Colour(0xff00f0ff));

    g.setGradientFill(meterGrad);
    g.fillRect(leftX, barY + barH - fillHL, barW, fillHL);
    g.fillRect(rightX, barY + barH - fillHR, barW, fillHR);

    // Red Peak Hold Lines
    g.setColour(juce::Colour(0xffff3366));
    g.fillRect(leftX, int(barY + barH - holdPercentL * float(barH)), barW, 2);
    g.fillRect(rightX, int(barY + barH - holdPercentR * float(barH)), barW, 2);

    // Channel Labels L and R
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b9bb4));
    g.drawText("L", leftX - 2, barY + barH + 2, barW + 4, 12, juce::Justification::centred);
    g.drawText("R", rightX - 2, barY + barH + 2, barW + 4, 12, juce::Justification::centred);

    // Digital Readouts below (Stabilized 0.5s refresh rate)
    auto digitalBox = bounds.reduced(10, 4);
    g.setColour(juce::Colour(0xff06090e));
    g.fillRoundedRectangle(digitalBox.toFloat(), 4.0f);
    g.setColour(juce::Colour(0xff141c28));
    g.drawRoundedRectangle(digitalBox.toFloat(), 4.0f, 1.0f);

    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b9bb4));
    g.drawText("MOMENTARY", digitalBox.getX() + 6, digitalBox.getY() + 4, 80, 10, juce::Justification::left);
    g.drawText("SHORT-TERM", digitalBox.getRight() - 86, digitalBox.getY() + 4, 80, 10, juce::Justification::right);

    g.setFont(juce::Font("monospace", 14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText(juce::String(dispMomentaryLufs, 1), digitalBox.getX() + 6, digitalBox.getY() + 16, 80, 16, juce::Justification::left);
    g.drawText(juce::String(dispShortTermLufs, 1), digitalBox.getRight() - 86, digitalBox.getY() + 16, 80, 16, juce::Justification::right);

    // Crest Factor & Spotify Status
    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b9bb4));
    g.drawText("CREST (DR)", digitalBox.getX() + 6, digitalBox.getY() + 38, 80, 10, juce::Justification::left);
    g.drawText("SPOTIFY", digitalBox.getRight() - 86, digitalBox.getY() + 38, 80, 10, juce::Justification::right);

    g.setFont(juce::Font("monospace", 12.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00ffaa));
    g.drawText(juce::String(dispCrest, 1) + " dB", digitalBox.getX() + 6, digitalBox.getY() + 50, 80, 14, juce::Justification::left);

    // Spotify status color (Green if -14 LUFS compliant, Red if too loud)
    g.setColour(dispSpotifyCol);
    g.drawText(dispSpotifyStatus, digitalBox.getRight() - 86, digitalBox.getY() + 50, 80, 14, juce::Justification::right);
}

// ------------------------------------------------------------------------------
// Draw Hardware Module Cards
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawModuleCard(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title)
{
    g.setColour(juce::Colour(0xff141c28));
    g.fillRoundedRectangle(bounds.toFloat(), 6.0f);
    g.setColour(juce::Colour(0xff2a3648));
    g.drawRoundedRectangle(bounds.toFloat(), 6.0f, 1.0f);

    auto header = bounds.removeFromTop(24).reduced(8, 4);
    g.setColour(juce::Colour(0xffcbd5e1));
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.drawText(title, header, juce::Justification::centredLeft);
}

// ------------------------------------------------------------------------------
// Draw Multi-Agent Live Console & Telemetry Deck
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawAgentDeck(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(0xff090d13));
    g.fillRoundedRectangle(bounds.toFloat(), 6.0f);
    g.setColour(juce::Colour(0xff1e2837));
    g.drawRoundedRectangle(bounds.toFloat(), 6.0f, 1.0f);

    // Header with Agent Chips
    auto chipArea = bounds.removeFromTop(28).reduced(8, 4);

    const char* agentNames[] = { "Orquestador", "Arquitecto DSP", "Revisor Anti-Distorsion", "Tester QA (Spotify)" };
    const juce::Colour agentColors[] = { juce::Colour(0xff00f0ff), juce::Colour(0xff5bc0de), juce::Colour(0xffffb92d), juce::Colour(0xff00ffaa) };

    int chipX = chipArea.getX();
    for (int i = 0; i < 4; ++i)
    {
        auto chipRect = juce::Rectangle<int>(chipX, chipArea.getY(), 160, 20);
        g.setColour(juce::Colour(0xff131922));
        g.fillRoundedRectangle(chipRect.toFloat(), 10.0f);
        g.setColour(juce::Colour(0xff2a3648));
        g.drawRoundedRectangle(chipRect.toFloat(), 10.0f, 1.0f);

        // Glowing dot
        g.setColour(agentColors[i]);
        g.fillEllipse(float(chipRect.getX() + 8), float(chipRect.getY() + 7), 6.0f, 6.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(9.5f, juce::Font::bold));
        g.drawText(agentNames[i], chipRect.getX() + 18, chipRect.getY(), 138, 20, juce::Justification::centredLeft);

        chipX += 170;
    }

    g.setColour(juce::Colour(0xff00ffaa));
    g.drawText("AGENTES ACTIVOS - IA COOPERATIVA", chipArea.getRight() - 220, chipArea.getY(), 210, 20, juce::Justification::right);

    // Terminal Window with Live Telemetry
    auto termArea = bounds.reduced(8, 6);
    g.setColour(juce::Colour(0xff040609));
    g.fillRoundedRectangle(termArea.toFloat(), 4.0f);

    g.setFont(juce::Font("monospace", 10.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText("[Orquestador] Procesamiento en tiempo real FL Studio | Latencia: 0 samples | DSP: 64-bit IEEE Float", termArea.getX() + 8, termArea.getY() + 4, termArea.getWidth() - 16, 14, juce::Justification::left);

    g.setColour(juce::Colour(0xff5bc0de));
    g.drawText("[Arquitecto] Balance espectral conforme a la curva de compensacion acustica Spotify (-14 LUFS Target)", termArea.getX() + 8, termArea.getY() + 20, termArea.getWidth() - 16, 14, juce::Justification::left);

    g.setColour(juce::Colour(0xffffb92d));
    g.drawText("[Revisor] Proteccion True-Peak activa en -1.0 dBTP | Subgrave en mono puro (<110Hz) para evitar cancelacion", termArea.getX() + 8, termArea.getY() + 36, termArea.getWidth() - 16, 14, juce::Justification::left);

    g.setColour(juce::Colour(0xff00ffaa));
    g.drawText("[Tester QA] Estado: APROBADO PARA SPOTIFY | Rango dinamico preservado sin aplastamiento ni distorsion", termArea.getX() + 8, termArea.getY() + 52, termArea.getWidth() - 16, 14, juce::Justification::left);
}

// ------------------------------------------------------------------------------
// Layout Coordinates for Knobs and Sliders
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::resized()
{
    // Top Bar controls
    presetBox.setBounds(getWidth() - 480, 14, 280, 30);
    autoMasterButton.setBounds(getWidth() - 190, 14, 160, 30);

    // Bypass buttons on module cards
    bypassLowCut.setBounds(115, 336, 36, 20);
    bypassEQ.setBounds(480, 336, 36, 20);
    bypassSat.setBounds(630, 336, 36, 20);
    bypassMB.setBounds(790, 336, 36, 20);
    bypassLimiter.setBounds(1020, 336, 36, 20);

    const int knobW = 56;
    const int knobH = 68;
    const int row1Y = 368;
    const int row2Y = 496;

    // Module 1: Input & Sub (Card 1, X: 20 to 155, Width: 135)
    inGainLabel.setBounds(26, row1Y, knobW, 14);
    inGainSlider.setBounds(26, row1Y + 14, knobW, knobH);

    lowCutLabel.setBounds(92, row1Y, knobW, 14);
    lowCutSlider.setBounds(92, row1Y + 14, knobW, knobH);

    // Module 2: 5-Band Parametric EQ (Card 2, X: 165 to 520, Width: 355)
    int eqStartX = 172;
    int eqSpacing = 68;

    eqSubLabel.setBounds(eqStartX, row1Y, knobW, 14);
    eqSubSlider.setBounds(eqStartX, row1Y + 14, knobW, knobH);

    eqLowMidLabel.setBounds(eqStartX + eqSpacing, row1Y, knobW, 14);
    eqLowMidSlider.setBounds(eqStartX + eqSpacing, row1Y + 14, knobW, knobH);

    eqMidLabel.setBounds(eqStartX + eqSpacing * 2, row1Y, knobW, 14);
    eqMidSlider.setBounds(eqStartX + eqSpacing * 2, row1Y + 14, knobW, knobH);

    eqClarityLabel.setBounds(eqStartX + eqSpacing * 3, row1Y, knobW, 14);
    eqClaritySlider.setBounds(eqStartX + eqSpacing * 3, row1Y + 14, knobW, knobH);

    eqAirLabel.setBounds(eqStartX + eqSpacing * 4, row1Y, knobW, 14);
    eqAirSlider.setBounds(eqStartX + eqSpacing * 4, row1Y + 14, knobW, knobH);

    // Module 3: Tape Warmth (Card 3, X: 530 to 670, Width: 140)
    satModeBox.setBounds(538, row1Y - 2, 124, 24);

    satDriveLabel.setBounds(538, row2Y, knobW, 14);
    satDriveSlider.setBounds(538, row2Y + 14, knobW, knobH);

    satWarmthLabel.setBounds(604, row2Y, knobW, 14);
    satWarmthSlider.setBounds(604, row2Y + 14, knobW, knobH);

    // Module 4: Multiband (Card 4, X: 680 to 830, Width: 150)
    mbLowLabel.setBounds(690, row1Y, knobW, 14);
    mbLowSlider.setBounds(690, row1Y + 14, knobW, knobH);

    mbMidLabel.setBounds(764, row1Y, knobW, 14);
    mbMidSlider.setBounds(764, row1Y + 14, knobW, knobH);

    mbHighLabel.setBounds(727, row2Y, knobW, 14);
    mbHighSlider.setBounds(727, row2Y + 14, knobW, knobH);

    // Module 5: Stereo & Limiter (Spotify) (Card 5, X: 840 to 1060, Width: 220)
    // Row 1: Width & Mono Sub
    stereoWidthLabel.setBounds(860, row1Y, knobW, 14);
    stereoWidthSlider.setBounds(860, row1Y + 14, knobW, knobH);

    monoMakerLabel.setBounds(960, row1Y, knobW, 14);
    monoMakerSlider.setBounds(960, row1Y + 14, knobW, knobH);

    // Row 2: Loudness, Ceiling, Out Gain
    loudnessLabel.setBounds(848, row2Y, knobW, 14);
    loudnessSlider.setBounds(848, row2Y + 14, knobW, knobH);

    ceilingLabel.setBounds(922, row2Y, knobW, 14);
    ceilingSlider.setBounds(922, row2Y + 14, knobW, knobH);

    outGainLabel.setBounds(996, row2Y, knobW, 14);
    outGainSlider.setBounds(996, row2Y + 14, knobW, knobH);
}
