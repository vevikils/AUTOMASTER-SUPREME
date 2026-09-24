#include "PluginProcessor.h"
#include "PluginEditor.h"

SupremeVocalBusCompressorAudioProcessorEditor::SupremeVocalBusCompressorAudioProcessorEditor(
    SupremeVocalBusCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // 15% smaller: 870 x 410 px (Original was 1020 x 480 px)
    // Compact, elegant, ultra-ergonomic studio rack format
    setSize(870, 410);

    // 1. Center Vintage VU Meter
    addAndMakeVisible(vuMeter);

    // 2. Configure Main Opto Knobs
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
    modeToggle.setButtonText("LIMIT (12:1)");
    modeToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffffffff));
    modeToggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffffcc00)); // Vivid Gold Tick
    modeToggle.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xff666666));
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

    // 6. 8 Signature Artist Presets
    presetBox.addItem("01. Travis Scott - Astroworld Vocal Glue", 1);
    presetBox.addItem("02. Drake - Silky OVO Bus Polish", 2);
    presetBox.addItem("03. T-Pain - Ultra Hard Auto-Tune Lock", 3);
    presetBox.addItem("04. Bad Bunny - Warm Latin Trap & Reggaeton", 4);
    presetBox.addItem("05. Rosalia - High Sheen Flamenco/Pop", 5);
    presetBox.addItem("06. Kendrick Lamar - Raw Punch & Dynamics", 6);
    presetBox.addItem("07. Billie Eilish - Whisper Intimacy & Air", 7);
    presetBox.addItem("08. The Weeknd - 80s Analog Retro Glow", 8);
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
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
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
        g.fillEllipse(sx - 8, sy - 8, 16, 16);
        g.setColour(juce::Colour(0xff707070));
        g.drawEllipse(sx - 8, sy - 8, 16, 16, 1.5f);
        // Screw slot
        g.setColour(juce::Colour(0xff151515));
        g.drawLine(sx - 4, sy, sx + 4, sy, 1.8f);
    };

    drawScrew(18.0f, 18.0f);
    drawScrew((float)w - 18.0f, 18.0f);
    drawScrew(18.0f, (float)h - 18.0f);
    drawScrew((float)w - 18.0f, (float)h - 18.0f);

    // 3. Sturdy Rack Handles on Left & Right
    g.setColour(juce::Colour(0xff222222));
    g.fillRoundedRectangle(30.0f, 55.0f, 8.0f, (float)h - 110.0f, 4.0f);
    g.fillRoundedRectangle((float)w - 38.0f, 55.0f, 8.0f, (float)h - 110.0f, 4.0f);
    g.setColour(juce::Colour(0xff777777));
    g.drawRoundedRectangle(30.0f, 55.0f, 8.0f, (float)h - 110.0f, 4.0f, 1.5f);
    g.drawRoundedRectangle((float)w - 38.0f, 55.0f, 8.0f, (float)h - 110.0f, 4.0f, 1.5f);

    // 4. Header Badges & Serigraphy
    g.setFont(juce::Font("Impact", 21.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff151515));
    g.drawText("SUPREME VOCAL BUS", 52, 14, 250, 24, juce::Justification::left);

    g.setFont(juce::Font("Helvetica", 12.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff8b1e1e));
    g.drawText("OPTO COMPRESSOR & HARMONIC EXCITER", 52, 36, 330, 16, juce::Justification::left);

    g.setFont(juce::Font("Helvetica", 9.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff4a4a4a));
    g.drawText("VERSION 1.3 - T4B CELL - BY VEVI - VOCAL GLUE & TUBE SHEEN", 52, 53, 380, 14, juce::Justification::left);

    // Preset Section Label
    g.drawText("ARTIST PRESETS:", w - 380, 18, 100, 20, juce::Justification::right);

    // 5. Left & Right Main Knob Legend Headers
    g.setFont(juce::Font("Impact", 17.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawText("OUTPUT GAIN", 55, 96, 170, 20, juce::Justification::centred);
    g.drawText("PEAK REDUCTION", w - 225, 96, 170, 20, juce::Justification::centred);

    // Parameter Function Legends below Main Knobs
    g.setFont(juce::Font("Helvetica", 8.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff555555));
    g.drawText("[ MAKE-UP GAIN: 0 TO +38 dB ]", 55, 248, 170, 14, juce::Justification::centred);
    g.drawText("[ OPTICAL THRESHOLD & RATIO ]", w - 225, 248, 170, 14, juce::Justification::centred);

    // Circular Decals for Main Knobs
    auto drawKnobDecal = [&g](float cx, float cy, float radius) {
        g.setColour(juce::Colour(0xff333333));
        g.setFont(juce::Font("Helvetica", 8.5f, juce::Font::bold));
        for (int i = 0; i <= 100; i += 10)
        {
            float norm = (float)i / 100.0f;
            float angleDeg = -135.0f + norm * 270.0f;
            float rad = juce::degreesToRadians(angleDeg);

            float rTick1 = radius + 5.0f;
            float rTick2 = radius + (i % 20 == 0 ? 11.0f : 8.0f);
            float tx1 = cx + rTick1 * std::sin(rad);
            float ty1 = cy - rTick1 * std::cos(rad);
            float tx2 = cx + rTick2 * std::sin(rad);
            float ty2 = cy - rTick2 * std::cos(rad);

            g.drawLine(tx1, ty1, tx2, ty2, (i % 20 == 0) ? 2.0f : 1.0f);

            if (i % 20 == 0)
            {
                float rText = radius + 18.0f;
                float tx = cx + rText * std::sin(rad);
                float ty = cy - rText * std::cos(rad);
                g.drawText(juce::String(i), (int)tx - 12, (int)ty - 7, 24, 14, juce::Justification::centred);
            }
        }
    };

    drawKnobDecal(140.0f, 180.0f, 46.0f);
    drawKnobDecal((float)w - 140.0f, 180.0f, 46.0f);

    // 6. Right Panel: Mode Switch & Meter Mode
    auto modePanel = juce::Rectangle<float>(w - 235.0f, 275.0f, 185.0f, 120.0f);
    g.setColour(juce::Colour(0xff1e1e1e));
    g.fillRoundedRectangle(modePanel, 6.0f);
    g.setColour(juce::Colour(0xff444444));
    g.drawRoundedRectangle(modePanel, 6.0f, 1.5f);

    g.setColour(juce::Colour(0xffe8ba35));
    g.setFont(juce::Font("Helvetica", 9.5f, juce::Font::bold));
    g.drawText("DYNAMICS MODE", (int)modePanel.getX(), (int)modePanel.getY() + 6, (int)modePanel.getWidth(), 14, juce::Justification::centred);
    g.setFont(juce::Font("Helvetica", 8.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xffaaaaaa));
    g.drawText("OFF: COMP (3:1) / ON: LIMIT", (int)modePanel.getX(), (int)modePanel.getY() + 20, (int)modePanel.getWidth(), 12, juce::Justification::centred);

    g.setFont(juce::Font("Helvetica", 9.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffe8ba35));
    g.drawText("METER SELECTOR", (int)modePanel.getX(), (int)modePanel.getY() + 64, (int)modePanel.getWidth(), 14, juce::Justification::centred);

    // 7. Left/Center Lower Rack Box: Harmonic Sheen & Tube Saturation Section
    auto sheenRect = juce::Rectangle<float>(50.0f, 275.0f, 570.0f, 120.0f);
    g.setColour(juce::Colour(0xff161616));
    g.fillRoundedRectangle(sheenRect, 6.0f);
    g.setColour(juce::Colour(0xffc59b27)); // Vintage Gold Trim
    g.drawRoundedRectangle(sheenRect, 6.0f, 1.5f);

    // Section Header Plate (A neat dedicated banner at the top of the box)
    auto bannerRect = juce::Rectangle<float>(sheenRect.getX() + 10.0f, sheenRect.getY() + 5.0f, sheenRect.getWidth() - 20.0f, 16.0f);
    g.setColour(juce::Colour(0xff222222));
    g.fillRoundedRectangle(bannerRect, 3.0f);
    g.setColour(juce::Colour(0xffe8ba35));
    g.setFont(juce::Font("Helvetica", 9.5f, juce::Font::bold));
    g.drawText("VOCAL GLUE HARMONIC SHEEN & TUBE EXCITER", bannerRect, juce::Justification::centred);

    // Parameter Legends strictly below the banner and below each knob
    // Knobs are centered at: 105, 217, 330, 442, 555
    struct ParamLegend {
        int x;
        const char* title;
        const char* subtitle;
    };

    ParamLegend legends[] = {
        { 105, "HARMONIC SHEEN", "AIR EXCITER" },
        { 217, "SHEEN FREQ",     "8k - 16k CUT" },
        { 330, "TUBE WARMTH",    "12AX7 TRIODE" },
        { 442, "HF SIDECHAIN",   "DE-HARSH TILT" },
        { 555, "PARALLEL MIX",   "DRY / WET %" }
    };

    for (const auto& l : legends)
    {
        // Parameter Title above knob (y=298)
        g.setColour(juce::Colour(0xffffffff));
        g.setFont(juce::Font("Helvetica", 8.5f, juce::Font::bold));
        g.drawText(l.title, l.x - 48, 298, 96, 12, juce::Justification::centred);

        // Subtitle below knob (y=380)
        g.setColour(juce::Colour(0xff999999));
        g.setFont(juce::Font("Helvetica", 7.5f, juce::Font::bold));
        g.drawText(l.subtitle, l.x - 48, 380, 96, 12, juce::Justification::centred);
    }
}

void SupremeVocalBusCompressorAudioProcessorEditor::resized()
{
    auto w = getWidth();

    // 1. Preset Box on Top Right
    presetBox.setBounds(w - 275, 16, 225, 24);

    // 2. Center Vintage VU Meter
    vuMeter.setBounds(w / 2 - 120, 70, 240, 160);

    // 3. Main Giant Opto Knobs (100 x 110 px)
    gainSlider.setBounds(90, 125, 100, 110);
    peakReductionSlider.setBounds(w - 190, 125, 100, 110);

    // 4. Mode Switch & Meter Box inside Right Panel
    modeToggle.setBounds(w - 225, 312, 165, 24);
    meterModeBox.setBounds(w - 225, 355, 165, 22);

    // 5. Lower Sheen & Tube Rack Knobs
    int startX = 75;
    int knobW = 60;
    int stepX = 112;

    sheenAmountSlider.setBounds(startX + stepX * 0, 312, knobW, 68);
    sheenFreqSlider.setBounds(startX + stepX * 1,   312, knobW, 68);
    tubeWarmthSlider.setBounds(startX + stepX * 2,  312, knobW, 68);
    hfEmphasisSlider.setBounds(startX + stepX * 3,  312, knobW, 68);
    dryWetSlider.setBounds(startX + stepX * 4,      312, knobW, 68);
}
