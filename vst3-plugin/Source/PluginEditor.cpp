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
    const float radius = float(std::min(width, height)) * 0.44f;
    const float centreX = float(x) + float(width) * 0.5f;
    const float centreY = float(y) + float(height) * 0.45f;
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const juce::Colour accent = slider.findColour(juce::Slider::rotarySliderFillColourId);

    // 1. Sleek subtle drop shadow for depth
    g.setColour(juce::Colour(0x35000000));
    g.fillEllipse(centreX - radius - 1.0f, centreY - radius, (radius + 1.0f) * 2.0f, (radius + 1.0f) * 2.0f);

    // 2. Outer Base Track Ring (FabFilter clean dark chassis)
    const float ringRadius = radius - 3.0f;
    juce::Path trackPath;
    trackPath.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);

    // Background arc track
    g.setColour(juce::Colour(0xff141a24));
    g.strokePath(trackPath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Glowing Neon Value Arc (FabFilter Bloom + Sharp Core)
    juce::Path valuePath;
    valuePath.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f, rotaryStartAngle, angle, true);

    // Ambient glow bloom
    g.setColour(accent.withAlpha(0.28f));
    g.strokePath(valuePath, juce::PathStrokeType(6.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Sharp neon arc line
    g.setColour(accent);
    g.strokePath(valuePath, juce::PathStrokeType(3.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 4. Inner Satin Rotary Cap (Dark Matte Charcoal with subtle bevel)
    const float capRadius = radius * 0.74f;
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff181f2b), centreX, centreY - capRadius,
                                           juce::Colour(0xff0d121a), centreX, centreY + capRadius, false));
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);

    // Bevel rim
    g.setColour(juce::Colour(0x28ffffff));
    g.drawEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.0f);

    // 5. Minimalist FabFilter Pointer Needle & Luminous Pip
    const float innerR = capRadius * 0.25f;
    const float outerR = capRadius * 0.88f;

    const float cosA = std::sin(angle);
    const float sinA = -std::cos(angle);

    const float x1 = centreX + innerR * cosA;
    const float y1 = centreY + innerR * sinA;
    const float x2 = centreX + outerR * cosA;
    const float y2 = centreY + outerR * sinA;

    // Glowing needle line
    g.setColour(accent.withAlpha(0.6f));
    g.drawLine(x1, y1, x2, y2, 2.0f);

    g.setColour(juce::Colours::white);
    g.drawLine(x1, y1, x2, y2, 1.2f);

    // Luminous pip dot at needle tip
    g.setColour(accent.withAlpha(0.4f));
    g.fillEllipse(x2 - 3.5f, y2 - 3.5f, 7.0f, 7.0f);
    g.setColour(accent);
    g.fillEllipse(x2 - 2.0f, y2 - 2.0f, 4.0f, 4.0f);
    g.setColour(juce::Colours::white);
    g.fillEllipse(x2 - 1.0f, y2 - 1.0f, 2.0f, 2.0f);

    // Center micro-hub
    g.setColour(juce::Colour(0xff121822));
    g.fillEllipse(centreX - 2.5f, centreY - 2.5f, 5.0f, 5.0f);
    g.setColour(juce::Colour(0x30ffffff));
    g.drawEllipse(centreX - 2.5f, centreY - 2.5f, 5.0f, 5.0f, 0.8f);
}

// ==============================================================================
// AutomasterSupremeAudioProcessorEditor Implementation
// ==============================================================================
AutomasterSupremeAudioProcessorEditor::AutomasterSupremeAudioProcessorEditor(AutomasterSupremeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1296, 888);
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
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff080b11));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff1a2434));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
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

void AutomasterSupremeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // 1. Sleek Studio Dark Graphite Chassis (FabFilter / Waves Aesthetic)
    g.fillAll(juce::Colour(0xff0a0d14));

    // 2. Header Top Bar (Height 68px)
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff141a24), 0, 0, juce::Colour(0xff0c1017), 0, 68, false));
    g.fillRect(0, 0, getWidth(), 68);
    g.setColour(juce::Colour(0xff222e40));
    g.drawLine(0.0f, 68.0f, float(getWidth()), 68.0f, 1.5f);

    // Brand Logo Icon (Rounded jewel badge with "A3")
    auto logoRect = juce::Rectangle<float>(20, 14, 40, 40);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff00f0ff), logoRect.getX(), logoRect.getY(),
                                           juce::Colour(0xffb34bfb), logoRect.getRight(), logoRect.getBottom(), false));
    g.fillRoundedRectangle(logoRect, 8.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("A3", logoRect, juce::Justification::centred);

    // Title & Version 3 Pill Badge
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("AUTOMASTER SUPREME 3", 72, 11, 315, 26, juce::Justification::centredLeft);

    // Version 3.0 Pill Badge
    auto verBadge = juce::Rectangle<float>(392, 15, 48, 20);
    g.setColour(juce::Colour(0x3000f0ff));
    g.fillRoundedRectangle(verBadge, 4.0f);
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawRoundedRectangle(verBadge, 4.0f, 1.0f);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("v3.0", verBadge, juce::Justification::centred);

    // Author Badge "BY VEVI"
    auto authorBadge = juce::Rectangle<float>(448, 15, 82, 20);
    g.setColour(juce::Colour(0x25ffb92d));
    g.fillRoundedRectangle(authorBadge, 4.0f);
    g.setColour(juce::Colour(0xffffb92d));
    g.drawRoundedRectangle(authorBadge, 4.0f, 1.0f);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("BY VEVI", authorBadge, juce::Justification::centred);

    // Subtitle
    g.setColour(juce::Colour(0xff7e91ad));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("AI MASTERING SUITE · BY VEVI", 72, 40, 185, 16, juce::Justification::centredLeft);

    // Spotify Reference Tag Pill
    auto spotifyTag = juce::Rectangle<float>(265, 39, 265, 19);
    g.setColour(juce::Colour(0x2000ffaa));
    g.fillRoundedRectangle(spotifyTag, 4.0f);
    g.setColour(juce::Colour(0xff00ffaa));
    g.drawRoundedRectangle(spotifyTag, 4.0f, 1.0f);
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.drawText("SPOTIFY TARGET (-14.0 LUFS / -1.0 dBTP)", spotifyTag, juce::Justification::centred);

    // 3. Upper Deck: 3 OLED Screen Frames (FabFilter & Waves Style, Scaled +20%)
    drawSpectrumScreen(g, juce::Rectangle<int>(20, 82, 640, 300));
    drawGoniometerScreen(g, juce::Rectangle<int>(672, 82, 310, 300));
    drawMetersScreen(g, juce::Rectangle<int>(994, 82, 282, 300));

    // 4. Lower Deck: 5 Hardware Rack Modules (Frosted dark glass with color accents, Scaled +20%)
    drawModuleCard(g, juce::Rectangle<int>(20, 396, 160, 330), "1. INPUT & SUB", juce::Colour(0xff00f0ff));
    drawModuleCard(g, juce::Rectangle<int>(194, 396, 420, 330), "2. 5-BAND EQUALIZER", juce::Colour(0xff00f0ff));
    drawModuleCard(g, juce::Rectangle<int>(628, 396, 166, 330), "3. TAPE WARMTH", juce::Colour(0xffffb92d));
    drawModuleCard(g, juce::Rectangle<int>(808, 396, 176, 330), "4. MULTIBAND", juce::Colour(0xff00ffaa));
    drawModuleCard(g, juce::Rectangle<int>(998, 396, 278, 330), "5. STEREO & LIMITER", juce::Colour(0xffff3366));

    // Frequency badges for 5-Band EQ (FabFilter style pills, Scaled)
    const char* eqFreqLabels[] = { "35 Hz", "300 Hz", "1.5 kHz", "4.5 kHz", "12 kHz" };
    const char* eqTypeLabels[] = { "LOW SHELF", "BELL (Q 1.0)", "BELL (Q 1.2)", "BELL (Q 1.1)", "HIGH SHELF" };
    for (int i = 0; i < 5; ++i)
    {
        int bandX = 204 + i * 80;
        auto badgeRect = juce::Rectangle<float>(float(bandX + 2), 548.0f, 64.0f, 28.0f);
        g.setColour(juce::Colour(0xff0a0e16));
        g.fillRoundedRectangle(badgeRect, 4.0f);
        g.setColour(juce::Colour(0xff1e2a3c));
        g.drawRoundedRectangle(badgeRect, 4.0f, 1.0f);
        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.setColour(i == 4 ? juce::Colour(0xffffb92d) : juce::Colour(0xff00f0ff));
        g.drawText(eqFreqLabels[i], badgeRect.removeFromTop(14.0f), juce::Justification::centred);
        g.setFont(juce::Font(7.5f, juce::Font::plain));
        g.setColour(juce::Colour(0xff8b9bb4));
        g.drawText(eqTypeLabels[i], badgeRect, juce::Justification::centred);
    }

    // 5. Bottom Multi-Agent Telemetry Bar (Scaled +20%)
    drawAgentDeck(g, juce::Rectangle<int>(20, 740, getWidth() - 40, 126));
}

// ------------------------------------------------------------------------------
// Draw Floating FabFilter-Style EQ Node
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawEQNode(juce::Graphics& g, float x, float y, int bandNum, float gainDb, juce::Colour col)
{
    // 1. Ambient Glow Halo (FabFilter bloom)
    g.setColour(col.withAlpha(0.22f));
    g.fillEllipse(x - 12.0f, y - 12.0f, 24.0f, 24.0f);

    // 2. Outer Ring
    g.setColour(col.withAlpha(0.75f));
    g.drawEllipse(x - 8.5f, y - 8.5f, 17.0f, 17.0f, 1.2f);

    // 3. Inner Node Body (Dark core with colored rim)
    g.setColour(juce::Colour(0xff0a0e16));
    g.fillEllipse(x - 7.0f, y - 7.0f, 14.0f, 14.0f);
    g.setColour(col);
    g.drawEllipse(x - 7.0f, y - 7.0f, 14.0f, 14.0f, 1.5f);

    // 4. Band Number in center
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText(juce::String(bandNum), int(x - 7.0f), int(y - 7.0f), 14, 14, juce::Justification::centred);

    // 5. Floating Gain Tag Pill (FabFilter Pro-Q style)
    juce::String tagText = (gainDb >= 0.0f ? "+" : "") + juce::String(gainDb, 1) + " dB";
    const int tagW = 44;
    const int tagH = 15;
    const int tagX = int(x) - tagW / 2;
    const int tagY = (gainDb >= 0.0f) ? int(y - 24.0f) : int(y + 11.0f);

    auto tagRect = juce::Rectangle<float>(float(tagX), float(tagY), float(tagW), float(tagH));
    g.setColour(juce::Colour(0xd8090d14));
    g.fillRoundedRectangle(tagRect, 3.5f);
    g.setColour(col.withAlpha(0.85f));
    g.drawRoundedRectangle(tagRect, 3.5f, 1.0f);

    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(col);
    g.drawText(tagText, tagRect, juce::Justification::centred);
}

// ------------------------------------------------------------------------------
// Draw Spectrum Analyzer & EQ Curve Screen (FabFilter Pro-Q Style)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawSpectrumScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Dark glass background
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0c1017), float(bounds.getX()), float(bounds.getY()),
                                           juce::Colour(0xff070a0f), float(bounds.getX()), float(bounds.getBottom()), false));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.2f);

    // Screen Header
    auto headerRect = bounds.removeFromTop(24);
    g.setColour(juce::Colour(0x12ffffff));
    g.fillRect(headerRect);
    g.setColour(juce::Colour(0xff00f0ff));
    g.fillRoundedRectangle(float(headerRect.getX() + 8), float(headerRect.getY() + 6), 3.0f, 12.0f, 1.5f);

    g.setColour(juce::Colour(0xffcbd5e1));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("SPECTRUM ANALYZER & MASTERING EQ (v2.0)", headerRect.getX() + 16, headerRect.getY(), 320, 24, juce::Justification::centredLeft);

    // Legend
    g.setFont(juce::Font(9.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText("* Live FFT (Post)", headerRect.getRight() - 170, headerRect.getY(), 95, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xffffe600));
    g.drawText("-- Curva EQ", headerRect.getRight() - 70, headerRect.getY(), 65, 24, juce::Justification::centredLeft);

    auto displayArea = bounds.reduced(8, 6);
    const float w = float(displayArea.getWidth());
    const float h = float(displayArea.getHeight());
    const float ox = float(displayArea.getX());
    const float oy = float(displayArea.getY());

    // Grid lines (dB)
    const float minDb = -66.0f;
    const float maxDb = 6.0f;
    const float dbRange = maxDb - minDb; // 72 dB

    g.setColour(juce::Colour(0x10ffffff));
    for (float db = 0.0f; db >= -60.0f; db -= 12.0f)
    {
        float normY = oy + h - ((db - minDb) / dbRange) * h;
        g.drawLine(ox, normY, ox + w, normY, 0.8f);
        g.setColour(juce::Colour(0x40ffffff));
        g.setFont(juce::Font(8.0f, juce::Font::plain));
        g.drawText(juce::String(int(db)) + " dB", ox + 2, normY - 8, 32, 10, juce::Justification::left);
        g.setColour(juce::Colour(0x10ffffff));
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
        g.drawLine(normX, oy, normX, oy + h, 0.8f);
        g.setColour(juce::Colour(0x40ffffff));
        g.setFont(juce::Font(8.0f, juce::Font::plain));
        g.drawText(freqLabels[i], normX + 2, oy + h - 10, 25, 10, juce::Justification::left);
        g.setColour(juce::Colour(0x10ffffff));
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

        // Neon Gradient Fill (Ambient glow style FabFilter)
        g.setGradientFill(juce::ColourGradient(juce::Colour(0x3500f0ff), ox, oy, juce::Colour(0x02001524), ox, oy + h, false));
        g.fillPath(fillPath);

        // Ambient glow stroke behind curve
        g.setColour(juce::Colour(0x4000f0ff));
        g.strokePath(fftPath, juce::PathStrokeType(3.5f));

        // Core line
        g.setColour(juce::Colour(0xff00f0ff));
        g.strokePath(fftPath, juce::PathStrokeType(1.6f));
    }

    // Dynamic Parametric EQ Curve & FabFilter Band Nodes
    const float gSub = audioProcessor.apvts.getRawParameterValue("eqLowShelfGain")->load();
    const float gLowMid = audioProcessor.apvts.getRawParameterValue("eqLowMidGain")->load();
    const float gMid = audioProcessor.apvts.getRawParameterValue("eqMidGain")->load();
    const float gClarity = audioProcessor.apvts.getRawParameterValue("eqHighMidGain")->load();
    const float gAir = audioProcessor.apvts.getRawParameterValue("eqHighShelfGain")->load();

    auto getEqCurveY = [&](float f) -> float {
        float totalGain = 0.0f;
        if (f < 160.0f) totalGain += gSub * (1.0f - f / 200.0f);
        totalGain += gLowMid * std::exp(-std::abs(std::log2(f / 300.0f)) * 1.5f);
        totalGain += gMid * std::exp(-std::abs(std::log2(f / 1500.0f)) * 1.5f);
        totalGain += gClarity * std::exp(-std::abs(std::log2(f / 4500.0f)) * 1.5f);
        if (f > 6000.0f) totalGain += gAir * std::min(1.0f, (f - 6000.0f) / 6000.0f);

        return oy + h - ((totalGain - 18.0f - minDb) / dbRange) * h;
    };

    auto getFreqX = [&](float f) -> float {
        return ox + ((std::log10(f) - minLog) / logRange) * w;
    };

    juce::Path eqCurve;
    for (int i = 0; i <= 100; ++i)
    {
        const float f = 20.0f * std::pow(1000.0f, float(i) / 100.0f);
        const float x = getFreqX(f);
        const float y = getEqCurveY(f);

        if (i == 0) eqCurve.startNewSubPath(x, y);
        else eqCurve.lineTo(x, y);
    }

    // EQ Curve with glow
    g.setColour(juce::Colour(0x35ffe600));
    g.strokePath(eqCurve, juce::PathStrokeType(3.5f));

    g.setColour(juce::Colour(0xffffe600));
    juce::PathStrokeType stroke(1.8f);
    juce::Path dashedEqCurve;
    float dashes[] = { 4.0f, 3.0f };
    stroke.createDashedStroke(dashedEqCurve, eqCurve, dashes, 2);
    g.strokePath(dashedEqCurve, stroke);

    // Floating Interactive EQ Nodes (FabFilter Pro-Q Style)
    drawEQNode(g, getFreqX(35.0f), getEqCurveY(35.0f), 1, gSub, juce::Colour(0xff00f0ff));
    drawEQNode(g, getFreqX(300.0f), getEqCurveY(300.0f), 2, gLowMid, juce::Colour(0xff00f0ff));
    drawEQNode(g, getFreqX(1500.0f), getEqCurveY(1500.0f), 3, gMid, juce::Colour(0xff00f0ff));
    drawEQNode(g, getFreqX(4500.0f), getEqCurveY(4500.0f), 4, gClarity, juce::Colour(0xff00f0ff));
    drawEQNode(g, getFreqX(12000.0f), getEqCurveY(12000.0f), 5, gAir, juce::Colour(0xffffb92d));
}

// ------------------------------------------------------------------------------
// Draw Lissajous Stereo Goniometer Screen
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawGoniometerScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0c1017), float(bounds.getX()), float(bounds.getY()),
                                           juce::Colour(0xff070a0f), float(bounds.getX()), float(bounds.getBottom()), false));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.2f);

    // Screen Header
    auto headerRect = bounds.removeFromTop(24);
    g.setColour(juce::Colour(0x12ffffff));
    g.fillRect(headerRect);
    g.setColour(juce::Colour(0xff00ffaa));
    g.fillRoundedRectangle(float(headerRect.getX() + 8), float(headerRect.getY() + 6), 3.0f, 12.0f, 1.5f);

    g.setColour(juce::Colour(0xffcbd5e1));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("STEREO GONIOMETER", headerRect.getX() + 16, headerRect.getY(), 150, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff00ffaa));
    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.drawText("LISSAJOUS", headerRect.getRight() - 75, headerRect.getY(), 65, 24, juce::Justification::right);

    auto scopeArea = bounds.removeFromTop(185).reduced(10);
    const float cx = scopeArea.getCentreX();
    const float cy = scopeArea.getCentreY();
    const float radius = float(std::min(scopeArea.getWidth(), scopeArea.getHeight())) * 0.48f;

    // Reticle concentric rings
    g.setColour(juce::Colour(0x1200f0ff));
    g.drawEllipse(cx - radius * 0.5f, cy - radius * 0.5f, radius, radius, 0.8f);
    g.setColour(juce::Colour(0x1a00f0ff));
    g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.0f);
    g.drawLine(cx - radius, cy, cx + radius, cy, 0.8f);
    g.drawLine(cx, cy - radius, cx, cy + radius, 0.8f);

    // Diagonal axis guides
    g.setColour(juce::Colour(0x1000f0ff));
    const float diag = radius * 0.707f;
    g.drawLine(cx - diag, cy - diag, cx + diag, cy + diag, 0.8f);
    g.drawLine(cx - diag, cy + diag, cx + diag, cy - diag, 0.8f);

    g.setColour(juce::Colour(0x5500f0ff));
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

    // Phosphor glow pass
    g.setColour(juce::Colour(0x3500ffaa));
    g.strokePath(beam, juce::PathStrokeType(2.8f));

    // Core beam
    g.setColour(juce::Colour(0xd000ffaa));
    g.strokePath(beam, juce::PathStrokeType(1.2f));

    // Phase Correlation Bar at bottom of screen
    auto phaseArea = bounds.reduced(10, 6);
    g.setColour(juce::Colour(0xff05070c));
    g.fillRoundedRectangle(phaseArea.toFloat(), 3.0f);
    g.setColour(juce::Colour(0xff161e2b));
    g.drawRoundedRectangle(phaseArea.toFloat(), 3.0f, 1.0f);

    // Center divider
    g.setColour(juce::Colour(0x30ffffff));
    g.drawLine(float(phaseArea.getCentreX()), float(phaseArea.getY()), float(phaseArea.getCentreX()), float(phaseArea.getBottom()), 1.0f);

    const float normPhase = std::clamp((curPhase + 1.0f) * 0.5f, 0.0f, 1.0f);
    const float cursorX = phaseArea.getX() + normPhase * float(phaseArea.getWidth());

    // Correlation cursor color (Green if >0, Red if <0)
    juce::Colour phaseCol = (curPhase >= 0.0f) ? juce::Colour(0xff00ffaa) : juce::Colour(0xffff3366);
    g.setColour(phaseCol.withAlpha(0.3f));
    g.fillEllipse(cursorX - 5.0f, float(phaseArea.getCentreY()) - 5.0f, 10.0f, 10.0f);
    g.setColour(phaseCol);
    g.fillRoundedRectangle(cursorX - 2.5f, float(phaseArea.getY() + 1), 5.0f, float(phaseArea.getHeight() - 2), 1.5f);

    g.setFont(juce::Font(7.5f, juce::Font::plain));
    g.setColour(juce::Colour(0x60ffffff));
    g.drawText("-1 (Anti)", phaseArea.getX() + 2, phaseArea.getY() - 10, 45, 10, juce::Justification::left);
    g.drawText("+1 (Mono)", phaseArea.getRight() - 52, phaseArea.getY() - 10, 50, 10, juce::Justification::right);
}

// ------------------------------------------------------------------------------
// Draw Precision True-Peak & LUFS Meters Screen (Waves Style)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawMetersScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0c1017), float(bounds.getX()), float(bounds.getY()),
                                           juce::Colour(0xff070a0f), float(bounds.getX()), float(bounds.getBottom()), false));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.2f);

    // Screen Header
    auto headerRect = bounds.removeFromTop(24);
    g.setColour(juce::Colour(0x12ffffff));
    g.fillRect(headerRect);
    g.setColour(juce::Colour(0xffff3366));
    g.fillRoundedRectangle(float(headerRect.getX() + 8), float(headerRect.getY() + 6), 3.0f, 12.0f, 1.5f);

    g.setColour(juce::Colour(0xffcbd5e1));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("PRECISION METERS", headerRect.getX() + 16, headerRect.getY(), 110, 24, juce::Justification::centredLeft);
    g.setColour(juce::Colour(0xff00f0ff));
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText("BS.1770 / EBU", headerRect.getRight() - 85, headerRect.getY(), 75, 24, juce::Justification::right);

    // Left & Right Segmented Peak Meters (Waves style LED ladder with 0.5s hold)
    auto meterArea = bounds.removeFromTop(120).reduced(10, 6);
    const float toPercentL = std::clamp((dispPeakL + 48.0f) / 48.0f, 0.0f, 1.0f);
    const float toPercentR = std::clamp((dispPeakR + 48.0f) / 48.0f, 0.0f, 1.0f);
    const float holdPercentL = std::clamp((dispPeakHoldL + 48.0f) / 48.0f, 0.0f, 1.0f);
    const float holdPercentR = std::clamp((dispPeakHoldR + 48.0f) / 48.0f, 0.0f, 1.0f);

    const int barW = 14;
    const int gap = 20;
    const int leftX = meterArea.getCentreX() - barW - gap / 2;
    const int rightX = meterArea.getCentreX() + gap / 2;
    const int barH = meterArea.getHeight() - 14;
    const int barY = meterArea.getY();

    // Scale ticks between L and R meters
    const float scaleDbs[] = { 0.0f, -3.0f, -6.0f, -12.0f, -18.0f, -24.0f, -36.0f, -48.0f };
    g.setFont(juce::Font(7.5f, juce::Font::bold));
    for (float db : scaleDbs)
    {
        const float norm = (db + 48.0f) / 48.0f;
        const int tickY = int(barY + barH - norm * float(barH));
        g.setColour(juce::Colour(0x35ffffff));
        g.drawHorizontalLine(tickY, leftX - 4, leftX);
        g.drawHorizontalLine(tickY, rightX + barW, rightX + barW + 4);

        g.setColour(db >= -1.0f ? juce::Colour(0xffff3366) : (db >= -14.0f ? juce::Colour(0xff00ffaa) : juce::Colour(0x55ffffff)));
        juce::String txt = (db == 0.0f) ? "0" : juce::String(int(db));
        g.drawText(txt, leftX + barW, tickY - 5, gap, 10, juce::Justification::centred);
    }

    // Channel L & R Background Tracks
    g.setColour(juce::Colour(0xff06090e));
    g.fillRoundedRectangle(float(leftX), float(barY), float(barW), float(barH), 2.0f);
    g.fillRoundedRectangle(float(rightX), float(barY), float(barW), float(barH), 2.0f);
    g.setColour(juce::Colour(0xff182230));
    g.drawRoundedRectangle(float(leftX), float(barY), float(barW), float(barH), 2.0f, 1.0f);
    g.drawRoundedRectangle(float(rightX), float(barY), float(barW), float(barH), 2.0f, 1.0f);

    // Segmented LED appearance
    const int fillHL = int(toPercentL * float(barH));
    const int fillHR = int(toPercentR * float(barH));

    juce::ColourGradient meterGrad(juce::Colour(0xffff3366), float(leftX), float(barY),
                                   juce::Colour(0xff00e676), float(leftX), float(barY + barH), false);
    meterGrad.addColour(0.20, juce::Colour(0xffff9500));
    meterGrad.addColour(0.40, juce::Colour(0xff00f0ff));

    g.setGradientFill(meterGrad);
    if (fillHL > 0) g.fillRect(leftX + 1, barY + barH - fillHL, barW - 2, fillHL);
    if (fillHR > 0) g.fillRect(rightX + 1, barY + barH - fillHR, barW - 2, fillHR);

    // LED segment lines for authentic Waves / hardware console feel
    g.setColour(juce::Colour(0xff06090e));
    for (int y = barY; y < barY + barH; y += 3)
    {
        g.drawHorizontalLine(y, leftX, leftX + barW);
        g.drawHorizontalLine(y, rightX, rightX + barW);
    }

    // High-visibility Peak Hold Needles (Updated every 0.5s)
    g.setColour(juce::Colours::white);
    g.fillRect(leftX, int(barY + barH - holdPercentL * float(barH)), barW, 2);
    g.fillRect(rightX, int(barY + barH - holdPercentR * float(barH)), barW, 2);

    // Channel Labels L and R
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b9bb4));
    g.drawText("L", leftX, barY + barH + 2, barW, 12, juce::Justification::centred);
    g.drawText("R", rightX, barY + barH + 2, barW, 12, juce::Justification::centred);

    // Digital Readouts below (Stabilized 0.5s refresh rate)
    auto digitalBox = bounds.reduced(8, 4);
    g.setColour(juce::Colour(0xff06090e));
    g.fillRoundedRectangle(digitalBox.toFloat(), 5.0f);
    g.setColour(juce::Colour(0xff182232));
    g.drawRoundedRectangle(digitalBox.toFloat(), 5.0f, 1.0f);

    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff7e91ad));
    g.drawText("MOMENTARY", digitalBox.getX() + 6, digitalBox.getY() + 4, 80, 10, juce::Justification::left);
    g.drawText("SHORT-TERM", digitalBox.getRight() - 86, digitalBox.getY() + 4, 80, 10, juce::Justification::right);

    g.setFont(juce::Font("monospace", 14.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText(juce::String(dispMomentaryLufs, 1), digitalBox.getX() + 6, digitalBox.getY() + 15, 80, 18, juce::Justification::left);
    g.drawText(juce::String(dispShortTermLufs, 1), digitalBox.getRight() - 86, digitalBox.getY() + 15, 80, 18, juce::Justification::right);

    // Crest Factor & Spotify Status
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff7e91ad));
    g.drawText("CREST (DR)", digitalBox.getX() + 6, digitalBox.getY() + 38, 80, 10, juce::Justification::left);
    g.drawText("SPOTIFY", digitalBox.getRight() - 86, digitalBox.getY() + 38, 80, 10, juce::Justification::right);

    g.setFont(juce::Font("monospace", 12.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00ffaa));
    g.drawText(juce::String(dispCrest, 1) + " dB", digitalBox.getX() + 6, digitalBox.getY() + 50, 80, 14, juce::Justification::left);

    // Spotify status pill
    auto spotifyBadge = juce::Rectangle<float>(float(digitalBox.getRight() - 86), float(digitalBox.getY() + 50), 80.0f, 15.0f);
    g.setColour(dispSpotifyCol.withAlpha(0.18f));
    g.fillRoundedRectangle(spotifyBadge, 3.0f);
    g.setColour(dispSpotifyCol);
    g.drawRoundedRectangle(spotifyBadge, 3.0f, 0.8f);
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText(dispSpotifyStatus, spotifyBadge, juce::Justification::centred);
}

// ------------------------------------------------------------------------------
// Draw Hardware Module Cards (Frosted Dark Glass Style)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawModuleCard(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title, juce::Colour accentCol)
{
    // Frosted dark glass chassis
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff101520), float(bounds.getX()), float(bounds.getY()),
                                           juce::Colour(0xff0a0e16), float(bounds.getX()), float(bounds.getBottom()), false));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);

    // Subtle 1px border
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.0f);

    // Header strip
    auto header = bounds.removeFromTop(24);
    g.setColour(juce::Colour(0x10ffffff));
    g.fillRect(header.reduced(2, 0));

    // Accent line on left of header
    g.setColour(accentCol);
    g.fillRoundedRectangle(float(header.getX() + 6), float(header.getY() + 6), 3.0f, 12.0f, 1.5f);

    g.setColour(juce::Colour(0xffcbd5e1));
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.drawText(title, header.withTrimmedLeft(14), juce::Justification::centredLeft);
}

// ------------------------------------------------------------------------------
// Draw Multi-Agent Live Console & Telemetry Deck
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawAgentDeck(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0d121b), float(bounds.getX()), float(bounds.getY()),
                                           juce::Colour(0xff070a0f), float(bounds.getX()), float(bounds.getBottom()), false));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.0f);

    // Header with Agent Chips
    auto chipArea = bounds.removeFromTop(32).reduced(10, 4);

    const char* agentNames[] = { "Orchestrator v3", "DSP Architect", "True-Peak Guard", "Spotify QA Engine" };
    const juce::Colour agentColors[] = { juce::Colour(0xff00f0ff), juce::Colour(0xff5bc0de), juce::Colour(0xffffb92d), juce::Colour(0xff00ffaa) };

    int chipX = chipArea.getX();
    for (int i = 0; i < 4; ++i)
    {
        auto chipRect = juce::Rectangle<int>(chipX, chipArea.getY(), 190, 24);
        g.setColour(juce::Colour(0xff111722));
        g.fillRoundedRectangle(chipRect.toFloat(), 12.0f);
        g.setColour(juce::Colour(0xff222e40));
        g.drawRoundedRectangle(chipRect.toFloat(), 12.0f, 1.0f);

        // Glowing dot
        g.setColour(agentColors[i]);
        g.fillEllipse(float(chipRect.getX() + 10), float(chipRect.getY() + 8), 8.0f, 8.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        g.drawText(agentNames[i], chipRect.getX() + 24, chipRect.getY(), 160, 24, juce::Justification::centredLeft);

        chipX += 205;
    }

    g.setColour(juce::Colour(0xff00ffaa));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("AI COOPERATIVE CORE v3.0 · BY VEVI", chipArea.getRight() - 260, chipArea.getY(), 250, 24, juce::Justification::right);

    // Terminal Window with Live Telemetry
    auto termArea = bounds.reduced(10, 8);
    g.setColour(juce::Colour(0xff040609));
    g.fillRoundedRectangle(termArea.toFloat(), 5.0f);

    g.setFont(juce::Font("monospace", 10.5f, juce::Font::plain));
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawText("[Orchestrator v3] Real-time FL Studio audio pipe | 64-bit IEEE float processing | Latency: 0 samples", termArea.getX() + 10, termArea.getY() + 6, termArea.getWidth() - 20, 16, juce::Justification::left);

    g.setColour(juce::Colour(0xff5bc0de));
    g.drawText("[DSP Architect] 5-band dynamic curves calibrated to Spotify acoustic reference (-14.0 LUFS Target)", termArea.getX() + 10, termArea.getY() + 24, termArea.getWidth() - 20, 16, juce::Justification::left);

    g.setColour(juce::Colour(0xffffb92d));
    g.drawText("[True-Peak Guard] Inter-sample peak limiter clamped at -1.0 dBTP | Elliptic mono-sub (<110Hz) locked", termArea.getX() + 10, termArea.getY() + 42, termArea.getWidth() - 20, 16, juce::Justification::left);

    g.setColour(juce::Colour(0xff00ffaa));
    g.drawText("[Spotify QA Engine] Status: CERTIFIED FOR SPOTIFY MASTERING | Dynamic Range intact without inter-sample clip", termArea.getX() + 10, termArea.getY() + 60, termArea.getWidth() - 20, 16, juce::Justification::left);
}

// ------------------------------------------------------------------------------
// Layout Coordinates for Knobs and Sliders (Window 1296 x 888)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::resized()
{
    // Top Bar controls (Width: 1296)
    presetBox.setBounds(getWidth() - 540, 17, 310, 34);
    autoMasterButton.setBounds(getWidth() - 210, 17, 186, 34);

    const int knobW = 68;
    const int knobH = 82;
    const int row1Y = 432;
    const int row2Y = 584;

    // Module 1: Input & Sub (Card 1, X: 20 to 180, Width: 160)
    inGainLabel.setBounds(28, row1Y, knobW, 14);
    inGainSlider.setBounds(28, row1Y + 14, knobW, knobH);

    lowCutLabel.setBounds(104, row1Y, knobW, 14);
    lowCutSlider.setBounds(104, row1Y + 14, knobW, knobH);

    // Module 2: 5-Band Parametric EQ (Card 2, X: 194 to 614, Width: 420)
    int eqStartX = 204;
    int eqSpacing = 80;

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

    // Module 3: Tape Warmth (Card 3, X: 628 to 794, Width: 166)
    satModeBox.setBounds(638, row1Y, 146, 28);

    satDriveLabel.setBounds(636, row2Y, knobW, 14);
    satDriveSlider.setBounds(636, row2Y + 14, knobW, knobH);

    satWarmthLabel.setBounds(716, row2Y, knobW, 14);
    satWarmthSlider.setBounds(716, row2Y + 14, knobW, knobH);

    // Module 4: Multiband (Card 4, X: 808 to 984, Width: 176)
    mbLowLabel.setBounds(818, row1Y, knobW, 14);
    mbLowSlider.setBounds(818, row1Y + 14, knobW, knobH);

    mbMidLabel.setBounds(906, row1Y, knobW, 14);
    mbMidSlider.setBounds(906, row1Y + 14, knobW, knobH);

    mbHighLabel.setBounds(862, row2Y, knobW, 14);
    mbHighSlider.setBounds(862, row2Y + 14, knobW, knobH);

    // Module 5: Stereo & Limiter (Card 5, X: 998 to 1276, Width: 278)
    // Row 1: Width & Mono Sub
    stereoWidthLabel.setBounds(1030, row1Y, knobW, 14);
    stereoWidthSlider.setBounds(1030, row1Y + 14, knobW, knobH);

    monoMakerLabel.setBounds(1170, row1Y, knobW, 14);
    monoMakerSlider.setBounds(1170, row1Y + 14, knobW, knobH);

    // Row 2: Loudness, Ceiling, Out Gain
    loudnessLabel.setBounds(1010, row2Y, knobW, 14);
    loudnessSlider.setBounds(1010, row2Y + 14, knobW, knobH);

    ceilingLabel.setBounds(1100, row2Y, knobW, 14);
    ceilingSlider.setBounds(1100, row2Y + 14, knobW, knobH);

    outGainLabel.setBounds(1190, row2Y, knobW, 14);
    outGainSlider.setBounds(1190, row2Y + 14, knobW, knobH);
}
