#include "PluginProcessor.h"
#include "PluginEditor.h"

SupremeVocalBusCompressorAudioProcessorEditor::SupremeVocalBusCompressorAudioProcessorEditor(
    SupremeVocalBusCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // CLA-2A dimensions: Authentic 19" Rack Proportions (940 x 420 px)
    setSize(940, 430);

    // 1. Add VU Meter
    addAndMakeVisible(vuMeter);

    // 2. Configure Main Opto Knobs
    configureKnob(peakReductionSlider, "PEAK REDUCTION");
    configureKnob(gainSlider, "GAIN");

    peakReductionAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "peak_reduction", peakReductionSlider);
    gainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "gain", gainSlider);

    // 3. Configure Sheen, Warmth & Sidechain Knobs
    configureKnob(sheenAmountSlider, "HARMONIC SHEEN");
    configureKnob(sheenFreqSlider, "SHEEN FREQ");
    configureKnob(tubeWarmthSlider, "TUBE WARMTH");
    configureKnob(hfEmphasisSlider, "HF EMPHASIS");
    configureKnob(dryWetSlider, "MIX");

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

    // 4. Compress / Limit Toggle Switch
    modeToggle.setButtonText("LIMIT");
    modeToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(0xff222222));
    addAndMakeVisible(modeToggle);
    modeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "mode", modeToggle);

    // 5. Meter Mode Box
    meterModeBox.addItem("GR", 1);
    meterModeBox.addItem("+4 IN", 2);
    meterModeBox.addItem("+10 OUT", 3);
    meterModeBox.setJustificationType(juce::Justification::centred);
    meterModeBox.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(meterModeBox);
    meterModeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "meter_mode", meterModeBox);

    // 6. Presets Menu
    presetBox.addItem("01. Vocal Glue Master (Universal Bus)", 1);
    presetBox.addItem("02. Modern Silk & Air (Pop / Trapsoul)", 2);
    presetBox.addItem("03. Heavy Opto Lock (Rap Lead & Adlibs)", 3);
    presetBox.addItem("04. Warm Tube Console (Vintage / Acoustic)", 4);
    presetBox.addItem("05. In-Your-Face Parallel (100% Glue / 50% Mix)", 5);
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

void SupremeVocalBusCompressorAudioProcessorEditor::configureKnob(juce::Slider& s, const juce::String& text)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 18);
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

    // 1. CLA-2A Classic Heavy Silver / Brushed Aluminum Chassis
    juce::ColourGradient chassisGrad(juce::Colour(0xffdedede), 0, 0,
                                     juce::Colour(0xffb5b5b5), 0, (float)h, false);
    g.setGradientFill(chassisGrad);
    g.fillAll();

    // Subtle brushed horizontal micro-texture
    g.setColour(juce::Colours::white.withAlpha(0.18f));
    for (int y = 0; y < h; y += 4)
        g.drawHorizontalLine(y, 0.0f, (float)w);

    // 2. 19" Rack Screws in 4 corners
    auto drawScrew = [&g](float sx, float sy) {
        g.setColour(juce::Colour(0xff333333));
        g.fillEllipse(sx - 9, sy - 9, 18, 18);
        g.setColour(juce::Colour(0xff777777));
        g.drawEllipse(sx - 9, sy - 9, 18, 18, 1.5f);
        // Screw slot
        g.setColour(juce::Colour(0xff1a1a1a));
        g.drawLine(sx - 5, sy, sx + 5, sy, 2.0f);
    };

    drawScrew(20.0f, 20.0f);
    drawScrew((float)w - 20.0f, 20.0f);
    drawScrew(20.0f, (float)h - 20.0f);
    drawScrew((float)w - 20.0f, (float)h - 20.0f);

    // 3. Rack Handles on Left & Right
    g.setColour(juce::Colour(0xff2b2b2b));
    g.fillRoundedRectangle(36.0f, 60.0f, 10.0f, (float)h - 120.0f, 4.0f);
    g.fillRoundedRectangle((float)w - 46.0f, 60.0f, 10.0f, (float)h - 120.0f, 4.0f);
    g.setColour(juce::Colour(0xff6e6e6e));
    g.drawRoundedRectangle(36.0f, 60.0f, 10.0f, (float)h - 120.0f, 4.0f, 1.5f);
    g.drawRoundedRectangle((float)w - 46.0f, 60.0f, 10.0f, (float)h - 120.0f, 4.0f, 1.5f);

    // 4. Faceplate Badges & Engraved Serigraphy
    g.setFont(juce::Font("Impact", 22.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xff181818));
    g.drawText("TELETRONIX", 65, 20, 180, 24, juce::Justification::left);

    g.setFont(juce::Font("Helvetica", 14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff881111));
    g.drawText("SUPREME VOCAL BUS COMPRESSOR", 65, 44, 300, 18, juce::Justification::left);

    g.setFont(juce::Font("Helvetica", 10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff444444));
    g.drawText("MODEL CLA-2A HYBRID · T4B OPTO CELL · BY VEVI", 65, 62, 340, 16, juce::Justification::left);

    // Preset Label on top right
    g.drawText("VOCAL BUS PRESETS:", w - 380, 24, 140, 18, juce::Justification::right);

    // 5. Classic CLA-2A Main Knob Circular Decal Rings
    auto drawKnobDecal = [&g](float cx, float cy, float radius, const juce::String& title) {
        g.setColour(juce::Colour(0xff222222));
        g.setFont(juce::Font("Impact", 18.0f, juce::Font::plain));
        g.drawText(title, (int)cx - 90, (int)cy - (int)radius - 34, 180, 22, juce::Justification::centred);

        // Number ticks 0 to 100
        g.setFont(juce::Font("Helvetica", 9.0f, juce::Font::bold));
        for (int i = 0; i <= 100; i += 10)
        {
            float norm = (float)i / 100.0f;
            float angleDeg = -135.0f + norm * 270.0f;
            float rad = juce::degreesToRadians(angleDeg);

            float rTick1 = radius + 6.0f;
            float rTick2 = radius + (i % 20 == 0 ? 14.0f : 10.0f);
            float tx1 = cx + rTick1 * std::sin(rad);
            float ty1 = cy - rTick1 * std::cos(rad);
            float tx2 = cx + rTick2 * std::sin(rad);
            float ty2 = cy - rTick2 * std::cos(rad);

            g.drawLine(tx1, ty1, tx2, ty2, (i % 20 == 0) ? 2.0f : 1.0f);

            if (i % 20 == 0)
            {
                float rText = radius + 22.0f;
                float tx = cx + rText * std::sin(rad);
                float ty = cy - rText * std::cos(rad);
                g.drawText(juce::String(i), (int)tx - 12, (int)ty - 7, 24, 14, juce::Justification::centred);
            }
        }
    };

    // Draw main decals
    drawKnobDecal(170.0f, 230.0f, 60.0f, "GAIN (OUTPUT)");
    drawKnobDecal((float)w - 170.0f, 230.0f, 60.0f, "PEAK REDUCTION");

    // 6. Center Lower Rack Box: Harmonic Sheen & Tube Saturation Section
    auto sheenRect = juce::Rectangle<float>(285.0f, 275.0f, 370.0f, 130.0f);
    g.setColour(juce::Colour(0xff222222));
    g.fillRoundedRectangle(sheenRect, 8.0f);
    g.setColour(juce::Colour(0xffc59b27)); // Vintage Gold Trim
    g.drawRoundedRectangle(sheenRect, 8.0f, 1.8f);

    // Section Title Badge
    g.setColour(juce::Colour(0xffc59b27));
    g.setFont(juce::Font("Helvetica", 11.0f, juce::Font::bold));
    g.drawText("★ VOCAL GLUE HARMONIC SHEEN & TUBE EXCITER ★",
               (int)sheenRect.getX(), (int)sheenRect.getY() + 6, (int)sheenRect.getWidth(), 16, juce::Justification::centred);

    // Labels for lower small knobs
    g.setFont(juce::Font("Helvetica", 9.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffe0e0e0));
    g.drawText("SHEEN", 300, 385, 60, 14, juce::Justification::centred);
    g.drawText("FREQ", 370, 385, 60, 14, juce::Justification::centred);
    g.drawText("TUBE", 440, 385, 60, 14, juce::Justification::centred);
    g.drawText("HF SIDE", 510, 385, 60, 14, juce::Justification::centred);
    g.drawText("MIX", 580, 385, 60, 14, juce::Justification::centred);

    // Mode Toggle Box Label
    g.setColour(juce::Colour(0xff222222));
    g.setFont(juce::Font("Helvetica", 10.0f, juce::Font::bold));
    g.drawText("COMPRESS / LIMIT", 665, 80, 120, 16, juce::Justification::centred);
    g.drawText("METER MODE", 670, 150, 110, 16, juce::Justification::centred);
}

void SupremeVocalBusCompressorAudioProcessorEditor::resized()
{
    auto w = getWidth();

    // 1. Preset Box on Top Right
    presetBox.setBounds(w - 230, 22, 210, 24);

    // 2. Center Vintage VU Meter
    vuMeter.setBounds(w / 2 - 135, 75, 270, 180);

    // 3. Main Giant Opto Knobs (CLA-2A Style: GAIN on left, PEAK REDUCTION on right)
    gainSlider.setBounds(100, 160, 140, 140);
    peakReductionSlider.setBounds(w - 240, 160, 140, 140);

    // 4. Toggle Mode & Meter ComboBox (Right of VU Meter)
    modeToggle.setBounds(685, 102, 85, 28);
    meterModeBox.setBounds(675, 172, 100, 24);

    // 5. Lower Sheen & Tube Rack Knobs
    int startX = 300;
    int knobW = 60;
    int gap = 10;
    sheenAmountSlider.setBounds(startX, 305, knobW, 75);
    sheenFreqSlider.setBounds(startX + (knobW + gap), 305, knobW, 75);
    tubeWarmthSlider.setBounds(startX + (knobW + gap) * 2, 305, knobW, 75);
    hfEmphasisSlider.setBounds(startX + (knobW + gap) * 3, 305, knobW, 75);
    dryWetSlider.setBounds(startX + (knobW + gap) * 4, 305, knobW, 75);
}
