#include "PluginProcessor.h"
#include "PluginEditor.h"

SupremeVocalBusCompressorAudioProcessorEditor::SupremeVocalBusCompressorAudioProcessorEditor(
    SupremeVocalBusCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Generous, professional 19" rack proportions (1020 x 480 px)
    // Ensures ZERO overlaps, clear spacing, legible decibels and legends
    setSize(1020, 480);

    // 1. Center Vintage VU Meter
    addAndMakeVisible(vuMeter);

    // 2. Configure Main Opto Knobs (No text overlay inside knob boundary)
    configureKnob(gainSlider, "GAIN", "dB");
    configureKnob(peakReductionSlider, "PEAK REDUCTION", "%");

    peakReductionAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "peak_reduction", peakReductionSlider);
    gainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "gain", gainSlider);

    // 3. Configure Sheen, Warmth & Sidechain Knobs
    configureKnob(sheenAmountSlider, "SHEEN", "%");
    configureKnob(sheenFreqSlider, "FREQ", "Hz");
    configureKnob(tubeWarmthSlider, "WARMTH", "%");
    configureKnob(hfEmphasisSlider, "HF SIDE", "");
    configureKnob(dryWetSlider, "MIX", "%");

    sheenAmountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "sheen_amount", sheenAmountSlider);
    sheenFreqAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "sheen_freq", sheenFreqSlider);
    tubeWarmthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "tube_warmth", tubeWarmthSlider);
    hfEmphasisAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "hf_emphasis", hfEmphasisSlider);
    dryWetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "dry_wet", dryWetSlider);

    // 4. Mode Toggle (Compress / Limit)
    // Clean styling with bright white readable text and high contrast checkmark
    modeToggle.setButtonText("LIMIT MODE (12:1)");
    modeToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xfff0f0f0));
    modeToggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffffcc00)); // Vivid Gold Tick
    modeToggle.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xff888888));
    addAndMakeVisible(modeToggle);
    modeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "mode", modeToggle);

    // 5. Meter Mode Box
    meterModeBox.addItem("GR (Gain Reduction)", 1);
    meterModeBox.addItem("+4 IN (Input Level)", 2);
    meterModeBox.addItem("+10 OUT (Output Level)", 3);
    meterModeBox.setJustificationType(juce::Justification::centred);
    meterModeBox.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(meterModeBox);
    meterModeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "meter_mode", meterModeBox);

    // 6. 5 Signature Artist Presets
    presetBox.addItem("01. Travis Scott - Astroworld Vocal Glue", 1);
    presetBox.addItem("02. Drake - Silky OVO Bus Polish", 2);
    presetBox.addItem("03. T-Pain - Ultra Hard Auto-Tune Lock", 3);
    presetBox.addItem("04. Bad Bunny - Warm Latin Trap & Reggaeton", 4);
    presetBox.addItem("05. Rosalia - High Sheen Flamenco/Pop", 5);
    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.onChange = [this]() {
        int idx = presetBox.getSelectedId() - 1;
        if (idx >= 0)
            audioProcessor.loadPreset(idx);
    };
    addAndMakeVisible(presetBox);

    startTimerHz(30); // 30 FPS smooth needle animation
}

SupremeVocalBusCompressorAudioProcessorEditor::~SupremeVocalBusCompressorAudioProcessorEditor()
{
    stopTimer();
}

void SupremeVocalBusCompressorAudioProcessorEditor::configureKnob(juce::Slider& s, const juce::String& text, const juce::String& suffix)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
    s.setTextValueSuffix(suffix.isNotEmpty() ? (" " + suffix) : "");
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff222222));
    s.setLookAndFeel(&knobLF);
    addAndMakeVisible(s);
}

void SupremeVocalBusCompressorAudioProcessorEditor::timerCallback()
{
    float gr = audioProcessor.getGainReductionDb();
    float inDb = audioProcessor.getInputMeterDb();
    float outDb = audioProcessor.getOutputMeterDb();
    int meterMode = meterModeBox.getSelectedId() - 1;
    if (meterMode < 0) meterMode = 0;

    vuMeter.setLevels(gr, inDb, outDb, meterMode);
}

void SupremeVocalBusCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto w = getWidth();
    auto h = getHeight();

    // 1. Brushed Aluminum / Studio Steel Faceplate
    juce::ColourGradient chassisGrad(juce::Colour(0xffe6e6e6), 0, 0,
                                     juce::Colour(0xffb8b8b8), 0, (float)h, false);
    g.setGradientFill(chassisGrad);
    g.fillAll();

    // Horizontal micro-machined texture
    g.setColour(juce::Colours::white.withAlpha(0.16f));
    for (int y = 0; y < h; y += 3)
        g.drawHorizontalLine(y, 0.0f, (float)w);

    // 2. Heavy Rack Screws in 4 corners
    auto drawScrew = [&g](float sx, float sy) {
        g.setColour(juce::Colour(0xff303030));
        g.fillEllipse(sx - 9, sy - 9, 18, 18);
        g.setColour(juce::Colour(0xff707070));
        g.drawEllipse(sx - 9, sy - 9, 18, 18, 1.5f);
        // Screw slot
        g.setColour(juce::Colour(0xff151515));
        g.drawLine(sx - 5, sy, sx + 5, sy, 2.0f);
    };

    drawScrew(22.0f, 22.0f);
    drawScrew((float)w - 22.0f, 22.0f);
    drawScrew(22.0f, (float)h - 22.0f);
    drawScrew((float)w - 22.0f, (float)h - 22.0f);

    // 3. Sturdy Rack Handles on Left & Right
    g.setColour(juce::Colour(0xff222222));
    g.fillRoundedRectangle(36.0f, 65.0f, 10.0f, (float)h - 130.0f, 4.0f);
    g.fillRoundedRectangle((float)w - 46.0f, 65.0f, 10.0f, (float)h - 130.0f, 4.0f);
    g.setColour(juce::Colour(0xff777777));
    g.drawRoundedRectangle(36.0f, 65.0f, 10.0f, (float)h - 130.0f, 4.0f, 1.5f);
    g.drawRoundedRectangle((float)w - 46.0f, 65.0f, 10.0f, (float)h - 130.0f, 4.0f, 1.5f);

    // 4. Header Badges & Serigraphy (Clean UTF-8 text, NO CLA-2A text)
    g.setFont(juce::Font("Impact", 24.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff151515));
    g.drawText("SUPREME VOCAL BUS", 65, 18, 280, 26, juce::Justification::left);

    g.setFont(juce::Font("Helvetica", 14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b1e1e));
    g.drawText("OPTO COMPRESSOR & HARMONIC EXCITER", 65, 44, 380, 18, juce::Justification::left);

    g.setFont(juce::Font("Helvetica", 10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff4a4a4a));
    g.drawText("VERSION 1.2 - T4B CELL - BY VEVI - VOCAL GLUE & TUBE SHEEN", 65, 64, 420, 16, juce::Justification::left);

    // Preset Section Label
    g.drawText("ARTIST PRESETS:", w - 410, 24, 110, 20, juce::Justification::right);

    // 5. Left & Right Main Knob Legend Headers
    g.setFont(juce::Font("Impact", 20.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawText("OUTPUT GAIN", 70, 112, 190, 22, juce::Justification::centred);
    g.drawText("PEAK REDUCTION", w - 260, 112, 190, 22, juce::Justification::centred);

    // Parameter Function Legends below Main Knobs
    g.setFont(juce::Font("Helvetica", 9.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff555555));
    g.drawText("[ MAKE-UP GAIN: 0 TO +38 dB ]", 70, 290, 190, 14, juce::Justification::centred);
    g.drawText("[ OPTICAL THRESHOLD & RATIO ]", w - 260, 290, 190, 14, juce::Justification::centred);

    // Circular Decals for Main Knobs (Calibrated spacing, no overlap)
    auto drawKnobDecal = [&g](float cx, float cy, float radius) {
        g.setColour(juce::Colour(0xff333333));
        g.setFont(juce::Font("Helvetica", 9.0f, juce::Font::bold));
        for (int i = 0; i <= 100; i += 10)
        {
            float norm = (float)i / 100.0f;
            float angleDeg = -135.0f + norm * 270.0f;
            float rad = juce::degreesToRadians(angleDeg);

            float rTick1 = radius + 6.0f;
            float rTick2 = radius + (i % 20 == 0 ? 13.0f : 9.0f);
            float tx1 = cx + rTick1 * std::sin(rad);
            float ty1 = cy - rTick1 * std::cos(rad);
            float tx2 = cx + rTick2 * std::sin(rad);
            float ty2 = cy - rTick2 * std::cos(rad);

            g.drawLine(tx1, ty1, tx2, ty2, (i % 20 == 0) ? 2.0f : 1.0f);

            if (i % 20 == 0)
            {
                float rText = radius + 21.0f;
                float tx = cx + rText * std::sin(rad);
                float ty = cy - rText * std::cos(rad);
                g.drawText(juce::String(i), (int)tx - 12, (int)ty - 7, 24, 14, juce::Justification::centred);
            }
        }
    };

    drawKnobDecal(165.0f, 212.0f, 54.0f);
    drawKnobDecal((float)w - 165.0f, 212.0f, 54.0f);

    // 6. Section Box: Mode Switch & Meter Mode (Dedicated right rack unit)
    auto modePanel = juce::Rectangle<float>(w - 270.0f, 320.0f, 210.0f, 140.0f);
    g.setColour(juce::Colour(0xff1e1e1e));
    g.fillRoundedRectangle(modePanel, 8.0f);
    g.setColour(juce::Colour(0xff444444));
    g.drawRoundedRectangle(modePanel, 8.0f, 1.5f);

    g.setColour(juce::Colour(0xffe8ba35));
    g.setFont(juce::Font("Helvetica", 10.5f, juce::Font::bold));
    g.drawText("DYNAMICS MODE", (int)modePanel.getX(), (int)modePanel.getY() + 10, (int)modePanel.getWidth(), 16, juce::Justification::centred);
    g.setFont(juce::Font("Helvetica", 8.5f, juce::Font::plain));
    g.setColour(juce::Colour(0xffaaaaaa));
    g.drawText("OFF: COMP (3:1) / ON: LIMIT (12:1)", (int)modePanel.getX(), (int)modePanel.getY() + 26, (int)modePanel.getWidth(), 14, juce::Justification::centred);

    g.setFont(juce::Font("Helvetica", 10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffe8ba35));
    g.drawText("METER SELECTOR", (int)modePanel.getX(), (int)modePanel.getY() + 74, (int)modePanel.getWidth(), 16, juce::Justification::centred);

    // 7. Center Lower Rack Box: Harmonic Sheen & Tube Saturation Section
    auto sheenRect = juce::Rectangle<float>(60.0f, 320.0f, 660.0f, 140.0f);
    g.setColour(juce::Colour(0xff181818));
    g.fillRoundedRectangle(sheenRect, 8.0f);
    g.setColour(juce::Colour(0xffc59b27)); // Vintage Gold Trim
    g.drawRoundedRectangle(sheenRect, 8.0f, 1.8f);

    // Section Header Title (Clean, NO text overlaps with knob labels)
    g.setColour(juce::Colour(0xffe8ba35));
    g.setFont(juce::Font("Helvetica", 11.5f, juce::Font::bold));
    g.drawText("VOCAL GLUE HARMONIC SHEEN & TUBE EXCITER",
               (int)sheenRect.getX(), (int)sheenRect.getY() + 10, (int)sheenRect.getWidth(), 16, juce::Justification::centred);

    // Dedicated Parameter Legends with technical explanation below each knob
    // Placed at y=332 (Title above knob) and y=438 (Subtitle below value readout)
    struct ParamLegend {
        int x;
        const char* title;
        const char* subtitle;
    };

    // Knobs are centered at: 110, 235, 360, 485, 610
    ParamLegend legends[] = {
        { 110, "HARMONIC SHEEN", "AIR EXCITER" },
        { 235, "SHEEN FREQ",     "8k - 16k CUT" },
        { 360, "TUBE WARMTH",    "12AX7 TRIODE" },
        { 485, "HF SIDECHAIN",   "DE-HARSH TILT" },
        { 610, "PARALLEL MIX",   "DRY / WET %" }
    };

    for (const auto& l : legends)
    {
        g.setColour(juce::Colour(0xfff0f0f0));
        g.setFont(juce::Font("Helvetica", 9.5f, juce::Font::bold));
        g.drawText(l.title, l.x - 55, 332, 110, 14, juce::Justification::centred);

        g.setColour(juce::Colour(0xffaaaaaa));
        g.setFont(juce::Font("Helvetica", 8.0f, juce::Font::bold));
        g.drawText(l.subtitle, l.x - 55, 442, 110, 14, juce::Justification::centred);
    }
}

void SupremeVocalBusCompressorAudioProcessorEditor::resized()
{
    auto w = getWidth();

    // 1. Preset Box on Top Right
    presetBox.setBounds(w - 330, 22, 260, 24);

    // 2. Center Vintage VU Meter
    vuMeter.setBounds(w / 2 - 145, 80, 290, 190);

    // 3. Main Giant Opto Knobs (Non-overlapping, clear bounds)
    gainSlider.setBounds(105, 148, 120, 130);
    peakReductionSlider.setBounds(w - 225, 148, 120, 130);

    // 4. Mode Switch & Meter Box inside Right Panel
    modeToggle.setBounds(w - 250, 364, 170, 26);
    meterModeBox.setBounds(w - 255, 412, 180, 24);

    // 5. Lower Sheen & Tube Rack Knobs
    int startX = 75;
    int knobW = 70;
    int stepX = 125;

    sheenAmountSlider.setBounds(startX + stepX * 0, 352, knobW, 82);
    sheenFreqSlider.setBounds(startX + stepX * 1,   352, knobW, 82);
    tubeWarmthSlider.setBounds(startX + stepX * 2,  352, knobW, 82);
    hfEmphasisSlider.setBounds(startX + stepX * 3,  352, knobW, 82);
    dryWetSlider.setBounds(startX + stepX * 4,      352, knobW, 82);
}
