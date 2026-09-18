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
    const bool dark = darkMode;
    const float radius = float(std::min(width, height)) * 0.44f;
    const float centreX = float(x) + float(width) * 0.5f;
    const float centreY = float(y) + float(height) * 0.45f;
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const juce::Colour accent = slider.findColour(juce::Slider::rotarySliderFillColourId);

    // 1. Sleek subtle drop shadow for depth
    g.setColour(dark ? juce::Colour(0x35000000) : juce::Colour(0x15000000));
    g.fillEllipse(centreX - radius - 1.0f, centreY - radius, (radius + 1.0f) * 2.0f, (radius + 1.0f) * 2.0f);

    // 2. Outer Base Track Ring
    const float ringRadius = radius - 3.0f;
    juce::Path trackPath;
    trackPath.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);

    // Background arc track
    g.setColour(dark ? juce::Colour(0xff141a24) : juce::Colour(0xffcbd5e1));
    g.strokePath(trackPath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 3. Glowing Neon Value Arc (FabFilter Bloom + Sharp Core)
    juce::Path valuePath;
    valuePath.addCentredArc(centreX, centreY, ringRadius, ringRadius, 0.0f, rotaryStartAngle, angle, true);

    // Ambient glow bloom
    g.setColour(accent.withAlpha(dark ? 0.28f : 0.22f));
    g.strokePath(valuePath, juce::PathStrokeType(6.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Sharp neon arc line
    g.setColour(accent);
    g.strokePath(valuePath, juce::PathStrokeType(3.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 4. Inner Satin Rotary Cap (Dark Matte Charcoal in dark, Platinum in light)
    const float capRadius = radius * 0.74f;
    if (dark)
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff181f2b), centreX, centreY - capRadius,
                                               juce::Colour(0xff0d121a), centreX, centreY + capRadius, false));
    }
    else
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffffffff), centreX, centreY - capRadius,
                                               juce::Colour(0xffe2e8f0), centreX, centreY + capRadius, false));
    }
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);

    // Bevel rim
    g.setColour(dark ? juce::Colour(0x28ffffff) : juce::Colour(0xffcbd5e1));
    g.drawEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.0f);

    // 5. Minimalist Pointer Needle & Luminous Pip
    const float innerR = capRadius * 0.25f;
    const float outerR = capRadius * 0.88f;

    const float cosA = std::sin(angle);
    const float sinA = -std::cos(angle);

    const float x1 = centreX + innerR * cosA;
    const float y1 = centreY + innerR * sinA;
    const float x2 = centreX + outerR * cosA;
    const float y2 = centreY + outerR * sinA;

    // Glowing needle line
    g.setColour(accent.withAlpha(dark ? 0.6f : 0.75f));
    g.drawLine(x1, y1, x2, y2, 2.0f);

    g.setColour(dark ? juce::Colours::white : juce::Colour(0xff1e293b));
    g.drawLine(x1, y1, x2, y2, 1.2f);

    // Luminous pip dot at needle tip
    g.setColour(accent.withAlpha(0.4f));
    g.fillEllipse(x2 - 3.5f, y2 - 3.5f, 7.0f, 7.0f);
    g.setColour(accent);
    g.fillEllipse(x2 - 2.0f, y2 - 2.0f, 4.0f, 4.0f);
    g.setColour(dark ? juce::Colours::white : juce::Colour(0xff1e293b));
    g.fillEllipse(x2 - 1.0f, y2 - 1.0f, 2.0f, 2.0f);

    // Center micro-hub
    g.setColour(dark ? juce::Colour(0xff121822) : juce::Colour(0xffe2e8f0));
    g.fillEllipse(centreX - 2.5f, centreY - 2.5f, 5.0f, 5.0f);
    g.setColour(dark ? juce::Colour(0x30ffffff) : juce::Colour(0xff94a3b8));
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

    // Theme Selector Button (Top Bar)
    themeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff151f2d));
    themeButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe2e8f0));
    themeButton.onClick = [this]()
    {
        setTheme(!isDarkMode);
    };
    addAndMakeVisible(themeButton);

    // Interactive Legend Button (Top Bar)
    legendButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff151f2d));
    legendButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffffb92d));
    legendButton.onClick = [this]()
    {
        showLegendDrawer = !showLegendDrawer;
        closeLegendButton.setVisible(showLegendDrawer);
        if (showLegendDrawer) closeLegendButton.toFront(true);
        repaint();
    };
    addAndMakeVisible(legendButton);

    // Close button for overlay drawer
    closeLegendButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff281118));
    closeLegendButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffff3366));
    closeLegendButton.onClick = [this]()
    {
        showLegendDrawer = false;
        closeLegendButton.setVisible(false);
        repaint();
    };
    closeLegendButton.setVisible(false);
    addChildComponent(closeLegendButton);

    // AI Auto Master Button
    autoMasterButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff0088cc));
    autoMasterButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    autoMasterButton.onClick = [this]()
    {
        audioProcessor.applyPreset(presetBox.getSelectedId() - 1);
    };
    addAndMakeVisible(autoMasterButton);

    // Reset Peaks Button (SPAN Dual Spectrum Header)
    resetPeaksButton.setButtonText("RESET PEAKS");
    resetPeaksButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0x35141e2d));
    resetPeaksButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffffaa00));
    resetPeaksButton.onClick = [this]()
    {
        audioProcessor.dsp.resetFFTMax();
        fftDisplayDataMax.fill(-100.0f);
        repaint(juce::Rectangle<int>(20, 82, 640, 300));
    };
    addAndMakeVisible(resetPeaksButton);

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

    // Register interactive legends for every control (100% English ASCII)
    registerLegend(inGainSlider, { "IN GAIN", "1. INPUT & SUB", "-18.0 dB to +18.0 dB", "0.0 dB",
        "Input gain trim before dynamic and tone shaping stages.",
        "Adjust so peak levels hover between -6 and -3 dBFS before EQ.", cyan });

    registerLegend(lowCutSlider, { "LOW CUT (Sub-Tamer)", "1. INPUT & SUB", "20 Hz to 80 Hz", "30 Hz (18dB/oct)",
        "High-pass filter removing inaudible sub-bass (<30Hz) that wastes limiter headroom.",
        "30 to 35 Hz cleans mud and prevents inter-sample overmodulation in Spotify.", gold });

    registerLegend(eqSubSlider, { "SUB BASS (80 Hz Low-Shelf)", "2. 5-BAND EQUALIZER", "-12.0 dB to +12.0 dB", "+0.5 dB",
        "Controls weight and fundamental punch for kick drum and synth sub bass.",
        "Subtle boosts (+0.5 to +1.5 dB) provide solid low-end without loudness penalties.", cyan });

    registerLegend(eqLowMidSlider, { "LOW-MID (250 Hz Mud Control)", "2. 5-BAND EQUALIZER", "-12.0 dB to +12.0 dB", "-0.8 dB",
        "Cleans boxy resonance, room boom, and low-mid buildup that muddies mixes.",
        "Gentle cuts (-0.5 to -1.5 dB) create immediate clarity and separate vocals from bass.", cyan });

    registerLegend(eqMidSlider, { "PRESENCE (1.2 kHz Vocal Body)", "2. 5-BAND EQUALIZER", "-12.0 dB to +12.0 dB", "+0.2 dB",
        "Central tone articulation for lead vocals, guitars, and snare body.",
        "Keep flat or apply gentle boost (+0.5 dB) for vocal clarity on mobile speakers.", cyan });

    registerLegend(eqClaritySlider, { "CLARITY (4.5 kHz Transient Bite)", "2. 5-BAND EQUALIZER", "-12.0 dB to +12.0 dB", "+0.8 dB",
        "Upper-mid articulation for vocal consonants, pick attacks, and crispness.",
        "+0.5 to +1.5 dB opens up the track without harshness or listening fatigue.", cyan });

    registerLegend(eqAirSlider, { "AIR SHEEN (12 kHz High-Shelf)", "2. 5-BAND EQUALIZER", "-12.0 dB to +12.0 dB", "+1.2 dB",
        "Adds ultra-high silky sheen and expensive modern studio dimension.",
        "+1.0 to +2.0 dB adds commercial open sparkle and wide headphone depth.", gold });

    registerLegend(satDriveSlider, { "TAPE DRIVE (Harmonic Exciter)", "3. TAPE WARMTH", "0% to 100%", "15%",
        "Generates analog tape harmonics to glue mix elements together seamlessly.",
        "10% to 25% provides warm analog cohesion without audible distortion.", gold });

    registerLegend(satWarmthSlider, { "ANALOG WARMTH", "3. TAPE WARMTH", "0% to 100%", "20%",
        "Smooths harsh transients and adds analog transformer weight to low frequencies.",
        "15% to 30% yields vintage console texture and rounder punch.", gold });

    registerLegend(satModeBox, { "SATURATION CIRCUIT", "3. TAPE WARMTH", "Tape / Tube / Console / Neve", "Analog Tape",
        "Analog modeling topology: Tape glue, Tube vocal warmth, Console, or Vintage Neve.",
        "Tape for general glue; Tube for vocal air; Neve for dense solid low-end.", gold });

    registerLegend(mbLowSlider, { "MB LOW DYNAMICS (<200 Hz)", "4. MULTIBAND", "-24.0 dB to 0.0 dB", "-6.0 dB",
        "Multiband compression clamping sub-bass energy and kick dynamics.",
        "Controls low-end energy so the final limiter operates with maximum transparency.", emerald });

    registerLegend(mbMidSlider, { "MB MID DYNAMICS (200 Hz - 4 kHz)", "4. MULTIBAND", "-24.0 dB to 0.0 dB", "-4.0 dB",
        "Maintains steady RMS consistency across vocal and mid instrumentation.",
        "1 to 2 dB gain reduction ensures consistent loudness across streaming playlists.", emerald });

    registerLegend(mbHighSlider, { "MB HIGH DYNAMICS (>4 kHz)", "4. MULTIBAND", "-24.0 dB to 0.0 dB", "-5.0 dB",
        "Controls cymbals and harsh sibilance before final true-peak limiting.",
        "Prevents harsh high-frequency distortion during lossy AAC/Ogg Vorbis encoding.", emerald });

    registerLegend(stereoWidthSlider, { "STEREO WIDTH (M/S Spatializer)", "5. STEREO & LIMITER", "0% (Mono) to 200% (Wide)", "110%",
        "Expands the stereo image in mids and highs while keeping center mono solid.",
        "105% to 120% recommended. Avoid exceeding 135% to prevent mono phase cancellation.", cyan });

    registerLegend(monoMakerSlider, { "MONO SUB (Mono-Maker 110Hz)", "5. STEREO & LIMITER", "20 Hz to 300 Hz", "110 Hz",
        "Collapses all frequencies below cutoff to strict mono (Side channel = 0).",
        "100 to 120 Hz is golden standard: massive punch without phase issues in clubs/mobile.", cyan });

    registerLegend(loudnessSlider, { "LOUDNESS DRIVE (Intelligent Maximizer)", "5. STEREO & LIMITER", "0.0 dB to +14.0 dB", "+3.5 dB",
        "Maximizes perceived RMS and integrated LUFS loudness transparently.",
        "Push during the loudest section until Short-Term LUFS reads exactly -14.0 LUFS.", red });

    registerLegend(ceilingSlider, { "TRUE-PEAK CEILING", "5. STEREO & LIMITER", "-3.0 dBTP to 0.0 dBTP", "-1.0 dBTP",
        "Absolute wall preventing inter-sample peaks during D/A conversion and compression.",
        "-1.0 dBTP is mandatory for Spotify to avoid clipping on streaming playback.", red });

    registerLegend(outGainSlider, { "OUTPUT TRIM", "5. STEREO & LIMITER", "-12.0 dB to +6.0 dB", "0.0 dB",
        "Final master output trim post-limiter.",
        "Leave at 0.0 dB to preserve true-peak limiter calibration.", cyan });

    registerLegend(presetBox, { "PRESET SELECTOR", "MASTER CONSOLE", "5 Calibration Profiles", "Spotify Standard",
        "Pre-calibrated profiles tailored to streaming standards per genre.",
        "Select the profile closest to your track genre as a verified starting point.", cyan });

    registerLegend(autoMasterButton, { "AI AUTO-MASTER ENGINE", "AI COOPERATIVE CORE", "Automated", "One-Click",
        "Automatically calibrates EQ curves and loudness to target Spotify reference.",
        "Play your track chorus or drop and click for instant mastering calibration.", emerald });

    registerLegend(legendButton, { "CONTROL GUIDE", "INSPECTOR", "Full Manual", "Modal",
        "Opens the interactive guide with the full matrix of modules and Spotify parameters.",
        "Click anytime to reference optimal parameter targets and signal routing.", gold });

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

    setTheme(true);
    startTimerHz(60);
}

AutomasterSupremeAudioProcessorEditor::~AutomasterSupremeAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void AutomasterSupremeAudioProcessorEditor::setTheme(bool dark)
{
    isDarkMode = dark;
    customKnobLAF.setDarkMode(dark);

    themeButton.setButtonText(dark ? "THEME: DARK" : "THEME: LIGHT");
    themeButton.setColour(juce::TextButton::buttonColourId, dark ? juce::Colour(0xff151f2d) : juce::Colour(0xffffffff));
    themeButton.setColour(juce::TextButton::textColourOffId, dark ? juce::Colour(0xffe2e8f0) : juce::Colour(0xff0f172a));

    legendButton.setColour(juce::TextButton::buttonColourId, dark ? juce::Colour(0xff151f2d) : juce::Colour(0xffffffff));
    legendButton.setColour(juce::TextButton::textColourOffId, dark ? juce::Colour(0xffffb92d) : juce::Colour(0xffb45309));

    resetPeaksButton.setColour(juce::TextButton::buttonColourId, dark ? juce::Colour(0x35141e2d) : juce::Colour(0xfff1f5f9));
    resetPeaksButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffffaa00));

    presetBox.setColour(juce::ComboBox::backgroundColourId, dark ? juce::Colour(0xff0d131b) : juce::Colour(0xffffffff));
    presetBox.setColour(juce::ComboBox::outlineColourId, dark ? juce::Colour(0xff233144) : juce::Colour(0xff94a3b8));
    presetBox.setColour(juce::ComboBox::textColourId, dark ? juce::Colour(0xff00f0ff) : juce::Colour(0xff0284c7));

    satModeBox.setColour(juce::ComboBox::backgroundColourId, dark ? juce::Colour(0xff0a0d13) : juce::Colour(0xffffffff));
    satModeBox.setColour(juce::ComboBox::outlineColourId, dark ? juce::Colour(0xff2a3648) : juce::Colour(0xff94a3b8));
    satModeBox.setColour(juce::ComboBox::textColourId, dark ? juce::Colour(0xffffb92d) : juce::Colour(0xffb45309));

    const juce::Colour tbBg = dark ? juce::Colour(0xff080b11) : juce::Colour(0xffffffff);
    const juce::Colour tbOutline = dark ? juce::Colour(0xff1a2434) : juce::Colour(0xffcbd5e1);
    const juce::Colour tbText = dark ? juce::Colours::white : juce::Colour(0xff0f172a);
    const juce::Colour lblCol = dark ? juce::Colour(0xff94a3b8) : juce::Colour(0xff475569);

    juce::Slider* allSliders[] = {
        &inGainSlider, &lowCutSlider,
        &eqSubSlider, &eqLowMidSlider, &eqMidSlider, &eqClaritySlider, &eqAirSlider,
        &satDriveSlider, &satWarmthSlider,
        &mbLowSlider, &mbMidSlider, &mbHighSlider,
        &stereoWidthSlider, &monoMakerSlider,
        &loudnessSlider, &ceilingSlider, &outGainSlider
    };
    for (auto* s : allSliders)
    {
        s->setColour(juce::Slider::textBoxBackgroundColourId, tbBg);
        s->setColour(juce::Slider::textBoxOutlineColourId, tbOutline);
        s->setColour(juce::Slider::textBoxTextColourId, tbText);
    }

    juce::Label* allLabels[] = {
        &inGainLabel, &lowCutLabel,
        &eqSubLabel, &eqLowMidLabel, &eqMidLabel, &eqClarityLabel, &eqAirLabel,
        &satDriveLabel, &satWarmthLabel,
        &mbLowLabel, &mbMidLabel, &mbHighLabel,
        &stereoWidthLabel, &monoMakerLabel,
        &loudnessLabel, &ceilingLabel, &outGainLabel
    };
    for (auto* l : allLabels)
    {
        l->setColour(juce::Label::textColourId, lblCol);
    }

    repaint();
}

void AutomasterSupremeAudioProcessorEditor::registerLegend(juce::Component& comp, const ControlLegendInfo& info)
{
    controlLegends.push_back({ &comp, info });
    comp.addMouseListener(this, true);
    if (auto* sc = dynamic_cast<juce::SettableTooltipClient*>(&comp))
    {
        sc->setTooltip(info.name + " (" + info.moduleName + ")\n" + info.description + "\nTarget Spotify: " + info.spotifyTip);
    }
}

void AutomasterSupremeAudioProcessorEditor::updateHoverFromComponent(juce::Component* comp)
{
    if (comp == nullptr) return;

    for (const auto& pair : controlLegends)
    {
        if (comp == pair.first || pair.first->isParentOf(comp))
        {
            activeLegend = pair.second;
            if (auto* slider = dynamic_cast<juce::Slider*>(pair.first))
            {
                activeControlValue = juce::String(slider->getValue(), 1) + slider->getTextValueSuffix();
            }
            else if (auto* box = dynamic_cast<juce::ComboBox*>(pair.first))
            {
                activeControlValue = box->getText();
            }
            else
            {
                activeControlValue = "ACTIVE";
            }
            isControlActive = true;
            repaint();
            return;
        }
    }
}

void AutomasterSupremeAudioProcessorEditor::mouseEnter(const juce::MouseEvent& event)
{
    updateHoverFromComponent(event.eventComponent);
}

void AutomasterSupremeAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
    updateHoverFromComponent(event.eventComponent);

    auto mousePos = getMouseXYRelative().toFloat();
    auto spectrumArea = juce::Rectangle<float>(20, 82, 640, 300);
    auto graphArea = spectrumArea.reduced(12, 10);
    graphArea.removeFromTop(20);
    graphArea.removeFromBottom(6);

    if (graphArea.contains(mousePos))
    {
        spectrumHoverPos = mousePos;
        if (!isHoveringSpectrum)
            isHoveringSpectrum = true;
        repaint(spectrumArea.toNearestInt());
    }
    else if (isHoveringSpectrum)
    {
        isHoveringSpectrum = false;
        repaint(spectrumArea.toNearestInt());
    }
}

void AutomasterSupremeAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
    updateHoverFromComponent(event.eventComponent);
}

void AutomasterSupremeAudioProcessorEditor::mouseExit(const juce::MouseEvent& event)
{
    if (isHoveringSpectrum)
    {
        isHoveringSpectrum = false;
        repaint(juce::Rectangle<int>(20, 82, 640, 300));
    }

    if (!event.mods.isAnyMouseButtonDown())
    {
        auto pos = getMouseXYRelative();
        auto* hit = getComponentAt(pos);
        bool hitFound = false;
        for (const auto& pair : controlLegends)
        {
            if (hit == pair.first || (hit != nullptr && pair.first->isParentOf(hit)))
            {
                updateHoverFromComponent(hit);
                hitFound = true;
                break;
            }
        }
        if (!hitFound)
        {
            isControlActive = false;
            repaint();
        }
    }
}

void AutomasterSupremeAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (showLegendDrawer)
    {
        auto modal = getLocalBounds().reduced(60, 45);
        if (!modal.contains(event.getPosition()))
        {
            showLegendDrawer = false;
            closeLegendButton.setVisible(false);
            repaint();
        }
    }

    // Reset Peak Hold when clicking inside the SPAN spectrum
    auto pos = getMouseXYRelative();
    auto spectrumArea = juce::Rectangle<int>(20, 82, 640, 300);
    if (spectrumArea.contains(pos) && !resetPeaksButton.getBounds().contains(pos))
    {
        audioProcessor.dsp.resetFFTMax();
        fftDisplayDataMax.fill(-100.0f);
        repaint(spectrumArea);
    }
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
    slider.onValueChange = [this, &slider]()
    {
        if (isControlActive)
        {
            activeControlValue = juce::String(slider.getValue(), 1) + slider.getTextValueSuffix();
            repaint();
        }
    };
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

    audioProcessor.dsp.getFFTMagnitudes(fftDisplayDataAvg.data(), fftDisplayDataMax.data(), int(fftDisplayDataAvg.size()));
    audioProcessor.dsp.getScopeSamples(scopeL.data(), scopeR.data(), int(scopeL.size()));

    // 1. Live True-Peak Vertical Meters update at full 60 FPS (Fast attack, smooth studio ballistics release)
    if (livePeakL > dispPeakL)
        dispPeakL = livePeakL;
    else
        dispPeakL = dispPeakL * 0.90f + livePeakL * 0.10f; // Fast, responsive decay

    if (livePeakR > dispPeakR)
        dispPeakR = livePeakR;
    else
        dispPeakR = dispPeakR * 0.90f + livePeakR * 0.10f;

    // 60 FPS Peak Hold Indicators
    if (dispPeakL > dispPeakHoldL)
        dispPeakHoldL = dispPeakL;
    else
        dispPeakHoldL = std::max(-36.0f, dispPeakHoldL - 0.15f);

    if (dispPeakR > dispPeakHoldR)
        dispPeakHoldR = dispPeakR;
    else
        dispPeakHoldR = std::max(-36.0f, dispPeakHoldR - 0.15f);

    // 2. Numerical Readouts (LUFS, Crest Factor, Spotify Status) update every 0.5s (30 ticks at 60Hz)
    if (liveLufs > -90.0f)
    {
        accumMomentaryLufs += liveLufs;
        accumCrest += liveCrest;
        accumSampleCount++;
    }

    meterTimerTickCount++;
    if (meterTimerTickCount >= 30) // 30 ticks at 60 Hz = exactly 0.5 seconds
    {
        meterTimerTickCount = 0;

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

        // Spotify Certification Logic
        if (dispShortTermLufs > -13.0f)
        {
            dispSpotifyStatus = "LOUD PENALTY";
            dispSpotifyCol = juce::Colour(0xffff3366);
        }
        else if (dispShortTermLufs < -16.0f)
        {
            dispSpotifyStatus = "LOW LEVEL";
            dispSpotifyCol = juce::Colour(0xffffb92d);
        }
        else
        {
            dispSpotifyStatus = "-14 TARGET OK";
            dispSpotifyCol = juce::Colour(0xff00ffaa);
        }

        // Reset accumulator for next 0.5s cycle
        accumMomentaryLufs = 0.0f;
        accumCrest = 0.0f;
        accumSampleCount = 0;
    }

    agentAnimationTick++;
    repaint();
}

void AutomasterSupremeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // 1. Sleek Studio Chassis (Dark Obsidian or Light Platinum)
    g.fillAll(isDarkMode ? juce::Colour(0xff0a0d14) : juce::Colour(0xffeef2f7));

    // 2. Header Top Bar (Height 68px)
    if (isDarkMode)
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff141a24), 0, 0, juce::Colour(0xff0c1017), 0, 68, false));
    else
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffffffff), 0, 0, juce::Colour(0xfff1f5f9), 0, 68, false));

    g.fillRect(0, 0, getWidth(), 68);
    g.setColour(isDarkMode ? juce::Colour(0xff222e40) : juce::Colour(0xffcbd5e1));
    g.drawLine(0.0f, 68.0f, float(getWidth()), 68.0f, 1.5f);

    // Brand Logo Icon (Rounded jewel badge with "A3")
    auto logoRect = juce::Rectangle<float>(20, 14, 40, 40);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff00f0ff), logoRect.getX(), logoRect.getY(),
                                           juce::Colour(0xffb34bfb), logoRect.getRight(), logoRect.getBottom(), false));
    g.fillRoundedRectangle(logoRect, 8.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("A3", logoRect, juce::Justification::centred);

    // Title
    g.setColour(isDarkMode ? juce::Colours::white : juce::Colour(0xff0f172a));
    g.setFont(juce::Font(21.0f, juce::Font::bold));
    g.drawText("AUTOMASTER SUPREME 3.4", 72, 12, 285, 24, juce::Justification::centredLeft);

    // Version 3.4 Pill Badge
    auto verBadge = juce::Rectangle<float>(362, 14, 44, 20);
    g.setColour(isDarkMode ? juce::Colour(0x3000f0ff) : juce::Colour(0x200284c7));
    g.fillRoundedRectangle(verBadge, 4.0f);
    g.setColour(isDarkMode ? juce::Colour(0xff00f0ff) : juce::Colour(0xff0284c7));
    g.drawRoundedRectangle(verBadge, 4.0f, 1.0f);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("v3.4", verBadge, juce::Justification::centred);

    // Author Badge "BY VEVI"
    auto authorBadge = juce::Rectangle<float>(412, 14, 72, 20);
    g.setColour(isDarkMode ? juce::Colour(0x25ffb92d) : juce::Colour(0x20b45309));
    g.fillRoundedRectangle(authorBadge, 4.0f);
    g.setColour(isDarkMode ? juce::Colour(0xffffb92d) : juce::Colour(0xffb45309));
    g.drawRoundedRectangle(authorBadge, 4.0f, 1.0f);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("BY VEVI", authorBadge, juce::Justification::centred);

    // Subtitle (English, no special chars)
    g.setColour(isDarkMode ? juce::Colour(0xff7e91ad) : juce::Colour(0xff64748b));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("AI MASTERING SUITE | BY VEVI", 72, 38, 175, 16, juce::Justification::centredLeft);

    // Spotify Reference Tag Pill (Starts at 252, width 215, ends at 467 -> 37px clean gap before presetBox at 504)
    auto spotifyTag = juce::Rectangle<float>(252, 38, 215, 18);
    g.setColour(isDarkMode ? juce::Colour(0x2000ffaa) : juce::Colour(0x20059669));
    g.fillRoundedRectangle(spotifyTag, 4.0f);
    g.setColour(isDarkMode ? juce::Colour(0xff00ffaa) : juce::Colour(0xff059669));
    g.drawRoundedRectangle(spotifyTag, 4.0f, 1.0f);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("SPOTIFY TARGET (-14.0 LUFS / -1.0 dBTP)", spotifyTag, juce::Justification::centred);

    // 3. Upper Deck: 3 OLED Screen Frames (FabFilter & Waves Style, Scaled +20%)
    drawSpectrumScreen(g, juce::Rectangle<int>(20, 82, 640, 300));
    drawGoniometerScreen(g, juce::Rectangle<int>(674, 82, 310, 300));
    drawMetersScreen(g, juce::Rectangle<int>(994, 82, 282, 300));

    // 4. Middle Deck: 5 Hardware Module Cards
    drawModuleCard(g, juce::Rectangle<int>(20, 396, 160, 330), "1. INPUT & SUB", juce::Colour(0xff00f0ff));
    drawModuleCard(g, juce::Rectangle<int>(194, 396, 420, 330), "2. 5-BAND EQUALIZER", juce::Colour(0xff00f0ff));
    drawModuleCard(g, juce::Rectangle<int>(628, 396, 166, 330), "3. TAPE WARMTH", juce::Colour(0xffffb92d));
    drawModuleCard(g, juce::Rectangle<int>(808, 396, 176, 330), "4. MULTIBAND", juce::Colour(0xff00ffaa));
    drawModuleCard(g, juce::Rectangle<int>(998, 396, 278, 330), "5. STEREO & LIMITER", juce::Colour(0xffff3366));

    // EQ Frequency Pill Badges
    const char* eqFreqs[] = { "80 Hz", "250 Hz", "1.2 kHz", "4.5 kHz", "12 kHz" };
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    for (int i = 0; i < 5; ++i)
    {
        int bandX = 204 + i * 80;
        auto badge = juce::Rectangle<float>(float(bandX), 548.0f, 68.0f, 15.0f);
        g.setColour(isDarkMode ? juce::Colour(0x2000f0ff) : juce::Colour(0x200284c7));
        g.fillRoundedRectangle(badge, 3.0f);
        g.setColour(isDarkMode ? juce::Colour(0x5000f0ff) : juce::Colour(0x600284c7));
        g.drawRoundedRectangle(badge, 3.0f, 0.8f);
        g.setColour(isDarkMode ? juce::Colour(0xff80eeff) : juce::Colour(0xff0369a1));
        g.drawText(eqFreqs[i], badge, juce::Justification::centred);
    }

    // 5. Lower Deck: Multi-Agent AI Console & Interactive Control Legend HUD
    drawAgentDeck(g, juce::Rectangle<int>(20, 740, 1256, 126));
}

void AutomasterSupremeAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    if (showLegendDrawer)
    {
        drawLegendDrawerOverlay(g);
    }
}

// ------------------------------------------------------------------------------
// Draw Interactive Full Legend / Guide Overlay Drawer
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawLegendDrawerOverlay(juce::Graphics& g)
{
    // Backdrop dark dimming
    g.fillAll(juce::Colour(0xdc05080e));

    auto modal = getLocalBounds().reduced(50, 36);

    // Modal Chassis (Dark Frosted Glass with Neon Glow border)
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff111722), float(modal.getX()), float(modal.getY()),
                                           juce::Colour(0xff090d14), float(modal.getX()), float(modal.getBottom()), false));
    g.fillRoundedRectangle(modal.toFloat(), 12.0f);

    g.setColour(juce::Colour(0xffffb92d));
    g.drawRoundedRectangle(modal.toFloat(), 12.0f, 1.5f);

    // Header strip
    auto header = modal.removeFromTop(56).reduced(16, 8);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("MASTERING CONTROL GUIDE & PARAMETER LEGEND | AUTOMASTER SUPREME 3.4", header.getX(), header.getY(), 750, 24, juce::Justification::left);

    g.setColour(juce::Colour(0xff00ffaa));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("OFFICIAL SPOTIFY STREAMING CALIBRATION (-14.0 LUFS / -1.0 dBTP) | BY VEVI", header.getX(), header.getY() + 24, 600, 16, juce::Justification::left);

    // 5 Columns for 5 Modules
    auto contentArea = modal.reduced(14, 10);
    const int colW = (contentArea.getWidth() - 32) / 5;
    const int colH = contentArea.getHeight() - 40;

    struct ModuleGuide
    {
        const char* title;
        juce::Colour col;
        std::vector<std::pair<const char*, const char*>> items;
    };

    std::vector<ModuleGuide> guides = {
        { "1. INPUT & SUB", juce::Colour(0xff00f0ff), {
            { "IN GAIN", "Pre-processing level trim (-18 to +18 dB). Peak -6 to -3 dBFS." },
            { "LOW CUT", "High-pass sub filter (20 to 80 Hz). 30-35 Hz frees limiter headroom." }
        }},
        { "2. 5-BAND EQ", juce::Colour(0xff00f0ff), {
            { "SUB BASS (80Hz)", "Low-shelf. +0.5 to +1.5 dB adds solid kick/bass punch." },
            { "LOW-MID (250Hz)", "Mud control bell. -0.5 to -1.5 dB cleans boxy mix buildup." },
            { "PRESENCE (1.2k)", "Vocal body and snare snap. Keep neutral or +0.5 dB." },
            { "CLARITY (4.5k)", "Transient bite and articulation. +0.5 to +1.5 dB opens clarity." },
            { "AIR SHEEN (12k)", "High-shelf sheen. +1.0 to +2.0 dB adds expensive studio air." }
        }},
        { "3. TAPE WARMTH", juce::Colour(0xffffb92d), {
            { "TAPE DRIVE", "Harmonic exciter. 10-25% glues instruments with analog color." },
            { "WARMTH", "Low-end saturation. 15-30% smooths harsh transients." },
            { "CIRCUIT", "Tape (glue), Tube (vocals), Console, Neve (thick bass)." }
        }},
        { "4. MULTIBAND", juce::Colour(0xff00ffaa), {
            { "MB LOW (<200Hz)", "Sub and kick compression. 1-2 dB reduction prevents pumping." },
            { "MB MID (200-4k)", "Vocal core stability and RMS loudness consistency." },
            { "MB HIGH (>4kHz)", "Tames harsh cymbals before lossy AAC/Ogg Vorbis encoding." }
        }},
        { "5. STEREO & LIMIT", juce::Colour(0xffff3366), {
            { "STEREO WIDTH", "Mid/Side width. 105-120% optimal for streaming expansion." },
            { "MONO SUB (110Hz)", "Elliptic mono maker. Tight punch without phase cancellation." },
            { "LOUDNESS DRIVE", "Intelligent maximizer. Push until Short-Term reads -14.0 LUFS." },
            { "TRUE-PEAK CEIL", "-1.0 dBTP ceiling required by Spotify to stop DAC clip." },
            { "OUTPUT TRIM", "Final master trim (keep at 0.0 dB for strict calibration)." }
        }}
    };

    for (int c = 0; c < 5; ++c)
    {
        auto colRect = juce::Rectangle<int>(contentArea.getX() + c * (colW + 8), contentArea.getY(), colW, colH);
        g.setColour(juce::Colour(0xff0c111a));
        g.fillRoundedRectangle(colRect.toFloat(), 6.0f);
        g.setColour(guides[c].col.withAlpha(0.4f));
        g.drawRoundedRectangle(colRect.toFloat(), 6.0f, 1.0f);

        // Module title strip
        auto colHeader = colRect.removeFromTop(24);
        g.setColour(guides[c].col.withAlpha(0.18f));
        g.fillRect(colHeader);
        g.setColour(guides[c].col);
        g.setFont(juce::Font(10.5f, juce::Font::bold));
        g.drawText(guides[c].title, colHeader.reduced(8, 0), juce::Justification::centredLeft);

        // Parameters in this module
        int itemY = colRect.getY() + 8;
        for (const auto& item : guides[c].items)
        {
            g.setColour(juce::Colours::white);
            g.setFont(juce::Font(10.0f, juce::Font::bold));
            g.drawText(item.first, colRect.getX() + 8, itemY, colW - 16, 15, juce::Justification::left);

            g.setColour(juce::Colour(0xff94a3b8));
            g.setFont(juce::Font(9.0f, juce::Font::plain));
            g.drawFittedText(item.second, colRect.getX() + 8, itemY + 16, colW - 16, 46, juce::Justification::topLeft, 3);

            itemY += 66;
        }
    }

    // Footer Rule
    auto footerArea = juce::Rectangle<int>(contentArea.getX(), contentArea.getBottom() - 32, contentArea.getWidth(), 30);
    g.setColour(juce::Colour(0xff121b27));
    g.fillRoundedRectangle(footerArea.toFloat(), 5.0f);
    g.setColour(juce::Colour(0xff00ffaa));
    g.drawRoundedRectangle(footerArea.toFloat(), 5.0f, 0.8f);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("SPOTIFY GOLDEN RULE: Mastering louder than -14 LUFS triggers automatic attenuation penalty. Preserve dynamics and punch!",
               footerArea.reduced(10, 0), juce::Justification::centred);
}

// ------------------------------------------------------------------------------
// Draw Real-Time Spectrum Analyzer Screen (FabFilter Style)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawSpectrumScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Screen Chassis
    g.setColour(juce::Colour(0xff06090e));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.0f);

    // Graph Area (Leave top 22px for SPAN header and bottom 12px for frequencies)
    auto graphArea = bounds.reduced(12, 10);
    graphArea.removeFromTop(20);
    graphArea.removeFromBottom(6);

    // Horizontal dB grid lines
    const float dbMarks[] = { 12.0f, 6.0f, 0.0f, -6.0f, -12.0f, -18.0f, -24.0f };
    for (float db : dbMarks)
    {
        float y = juce::jmap(db, -28.0f, 18.0f, float(graphArea.getBottom()), float(graphArea.getY()));
        g.setColour(juce::Colour(0xff101824));
        g.drawLine(float(graphArea.getX()), y, float(graphArea.getRight()), y, 0.7f);

        g.setColour(juce::Colour(0xff33445b));
        g.setFont(juce::Font("monospace", 8.5f, juce::Font::plain));
        g.drawText(juce::String(int(db)) + "dB", graphArea.getX() + 2, int(y) - 10, 32, 10, juce::Justification::left);
    }

    // Vertical Frequency grid lines (Logarithmic)
    const float freqs[] = { 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f, 20000.0f };
    const char* freqLabels[] = { "50", "100", "250", "500", "1k", "2.5k", "5k", "10k", "20k" };

    for (int i = 0; i < 9; ++i)
    {
        float normX = std::log10(freqs[i] / 20.0f) / std::log10(20000.0f / 20.0f);
        float x = float(graphArea.getX()) + normX * float(graphArea.getWidth());
        g.setColour(juce::Colour(0xff101824));
        g.drawLine(x, float(graphArea.getY()), x, float(graphArea.getBottom()), 0.7f);

        g.setColour(juce::Colour(0xff33445b));
        g.setFont(juce::Font("monospace", 8.5f, juce::Font::plain));
        g.drawText(freqLabels[i], int(x) - 14, graphArea.getBottom() - 11, 28, 10, juce::Justification::centred);
    }

    const float sRate = audioProcessor.getSampleRate() > 0 ? float(audioProcessor.getSampleRate()) : 44100.0f;
    const int numBins = int(fftDisplayDataAvg.size());

    // 1. AVERAGE (AVG) SPECTRUM CURVE (Electric Cyan Fill & Stroke)
    juce::Path avgPath;
    avgPath.startNewSubPath(float(graphArea.getX()), float(graphArea.getBottom()));
    for (int i = 1; i < numBins; ++i)
    {
        float binFreq = (float(i) * sRate) / float(AudioConstants::FFT_SIZE);
        if (binFreq < 20.0f || binFreq > 20000.0f) continue;

        float normX = std::log10(binFreq / 20.0f) / std::log10(20000.0f / 20.0f);
        float px = float(graphArea.getX()) + normX * float(graphArea.getWidth());

        float magDb = fftDisplayDataAvg[size_t(i)];
        float py = juce::jmap(juce::jlimit(-70.0f, 12.0f, magDb), -70.0f, 12.0f, float(graphArea.getBottom()), float(graphArea.getY()));

        avgPath.lineTo(px, py);
    }
    avgPath.lineTo(float(graphArea.getRight()), float(graphArea.getBottom()));
    avgPath.closeSubPath();

    // AVG Translucent Gradient Fill
    g.setGradientFill(juce::ColourGradient(juce::Colour(0x3500d4ff), float(graphArea.getX()), float(graphArea.getY()),
                                           juce::Colour(0x02001525), float(graphArea.getX()), float(graphArea.getBottom()), false));
    g.fillPath(avgPath);

    // AVG Glowing Outline
    g.setColour(juce::Colour(0x5000f0ff));
    g.strokePath(avgPath, juce::PathStrokeType(2.4f));
    g.setColour(juce::Colour(0xee00f0ff));
    g.strokePath(avgPath, juce::PathStrokeType(1.2f));

    // 2. MAX PEAK (MAX) SPECTRUM CURVE (High-Contrast SPAN Warm Amber / Gold)
    juce::Path maxPath;
    bool maxStarted = false;
    for (int i = 1; i < numBins; ++i)
    {
        float binFreq = (float(i) * sRate) / float(AudioConstants::FFT_SIZE);
        if (binFreq < 20.0f || binFreq > 20000.0f) continue;

        float normX = std::log10(binFreq / 20.0f) / std::log10(20000.0f / 20.0f);
        float px = float(graphArea.getX()) + normX * float(graphArea.getWidth());

        float magDb = fftDisplayDataMax[size_t(i)];
        float py = juce::jmap(juce::jlimit(-70.0f, 12.0f, magDb), -70.0f, 12.0f, float(graphArea.getBottom()), float(graphArea.getY()));

        if (!maxStarted)
        {
            maxPath.startNewSubPath(px, py);
            maxStarted = true;
        }
        else
        {
            maxPath.lineTo(px, py);
        }
    }

    if (maxStarted)
    {
        // MAX Peak Stroke (Amber Glow Bloom + Crisp Line)
        g.setColour(juce::Colour(0x40ff9900));
        g.strokePath(maxPath, juce::PathStrokeType(2.8f));
        g.setColour(juce::Colour(0xffffaa00));
        g.strokePath(maxPath, juce::PathStrokeType(1.4f));
    }

    // 3. EQ Filter Curve Overlay (FabFilter Style Composite Curve)
    const float gains[] = {
        float(eqSubSlider.getValue()),
        float(eqLowMidSlider.getValue()),
        float(eqMidSlider.getValue()),
        float(eqClaritySlider.getValue()),
        float(eqAirSlider.getValue())
    };
    const float nodeFreqs[] = { 80.0f, 250.0f, 1200.0f, 4500.0f, 12000.0f };

    juce::Path eqCurvePath;
    bool pathStarted = false;

    for (int px = graphArea.getX(); px <= graphArea.getRight(); px += 2)
    {
        float normX = float(px - graphArea.getX()) / float(graphArea.getWidth());
        float f = 20.0f * std::pow(20000.0f / 20.0f, normX);

        // Low cut attenuation
        float lowCutFreq = float(lowCutSlider.getValue());
        float eqTotalDb = 0.0f;
        if (f < lowCutFreq)
        {
            float octavesBelow = std::log2(lowCutFreq / f);
            eqTotalDb -= octavesBelow * 18.0f; // 18 dB/oct HPF
        }

        // Add 5-band bells & shelves
        eqTotalDb += gains[0] / (1.0f + std::pow(f / nodeFreqs[0], 2.0f));
        float d2 = std::log2(f / nodeFreqs[1]);
        eqTotalDb += gains[1] * std::exp(-d2 * d2 * 1.8f);
        float d3 = std::log2(f / nodeFreqs[2]);
        eqTotalDb += gains[2] * std::exp(-d3 * d3 * 1.8f);
        float d4 = std::log2(f / nodeFreqs[3]);
        eqTotalDb += gains[3] * std::exp(-d4 * d4 * 2.2f);
        eqTotalDb += gains[4] / (1.0f + std::pow(nodeFreqs[4] / f, 2.0f));

        float py = juce::jmap(juce::jlimit(-24.0f, 18.0f, eqTotalDb), -28.0f, 18.0f, float(graphArea.getBottom()), float(graphArea.getY()));

        if (!pathStarted)
        {
            eqCurvePath.startNewSubPath(float(px), py);
            pathStarted = true;
        }
        else
        {
            eqCurvePath.lineTo(float(px), py);
        }
    }

    // Glow bloom for FabFilter Curve
    g.setColour(juce::Colour(0x40ffff55));
    g.strokePath(eqCurvePath, juce::PathStrokeType(3.5f));
    g.setColour(juce::Colour(0xfffff066));
    g.strokePath(eqCurvePath, juce::PathStrokeType(1.8f));

    // Draw FabFilter Interactive Visual Nodes (1 to 5)
    juce::Colour nodeColors[] = {
        juce::Colour(0xff00f0ff), juce::Colour(0xff00ffaa), juce::Colour(0xff33ccff),
        juce::Colour(0xffffb92d), juce::Colour(0xffff55aa)
    };
    for (int i = 0; i < 5; ++i)
    {
        float normX = std::log10(nodeFreqs[i] / 20.0f) / std::log10(20000.0f / 20.0f);
        float nx = float(graphArea.getX()) + normX * float(graphArea.getWidth());
        float ny = juce::jmap(juce::jlimit(-24.0f, 18.0f, gains[i]), -28.0f, 18.0f, float(graphArea.getBottom()), float(graphArea.getY()));

        drawEQNode(g, nx, ny, i + 1, gains[i], nodeColors[i]);
    }

    // 4. SPAN-Style Header Bar
    int hdrY = bounds.getY() + 6;
    g.setColour(juce::Colour(0xff00f0ff));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("SPAN DUAL SPECTRUM ANALYZER", bounds.getX() + 12, hdrY, 190, 16, juce::Justification::centredLeft);

    // Legend Pill 1: AVG (Cyan)
    auto avgPill = juce::Rectangle<float>(float(bounds.getX() + 208), float(hdrY), 78.0f, 17.0f);
    g.setColour(juce::Colour(0x2500f0ff));
    g.fillRoundedRectangle(avgPill, 3.0f);
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawRoundedRectangle(avgPill, 3.0f, 0.8f);
    g.fillRect(avgPill.getX() + 6.0f, avgPill.getY() + 5.0f, 7.0f, 7.0f);
    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.drawText("AVG (RMS)", avgPill.withTrimmedLeft(16), juce::Justification::centredLeft);

    // Legend Pill 2: MAX (Amber)
    auto maxPill = juce::Rectangle<float>(float(bounds.getX() + 292), float(hdrY), 78.0f, 17.0f);
    g.setColour(juce::Colour(0x25ffaa00));
    g.fillRoundedRectangle(maxPill, 3.0f);
    g.setColour(juce::Colour(0xffffaa00));
    g.drawRoundedRectangle(maxPill, 3.0f, 0.8f);
    g.fillRect(maxPill.getX() + 6.0f, maxPill.getY() + 5.0f, 7.0f, 7.0f);
    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.drawText("MAX PEAK", maxPill.withTrimmedLeft(16), juce::Justification::centredLeft);

    // Slope Badge
    auto slopeRect = juce::Rectangle<float>(float(bounds.getX() + 376), float(hdrY), 94.0f, 17.0f);
    g.setColour(juce::Colour(0x1833445b));
    g.fillRoundedRectangle(slopeRect, 3.0f);
    g.setColour(juce::Colour(0xff475569));
    g.drawRoundedRectangle(slopeRect, 3.0f, 0.7f);
    g.setColour(juce::Colour(0xff94a3b8));
    g.setFont(juce::Font(8.0f, juce::Font::plain));
    g.drawText("+3.5 dB SLOPE", slopeRect, juce::Justification::centred);

    // Live EQ Notice
    g.setColour(juce::Colour(0xfffff066));
    g.setFont(juce::Font(8.5f, juce::Font::bold));
    g.drawText("LIVE EQ", bounds.getX() + 478, hdrY, 52, 17, juce::Justification::centredLeft);

    // 5. SPAN Cursor Crosshair & Floating Note Inspector
    if (isHoveringSpectrum && graphArea.toFloat().contains(spectrumHoverPos))
    {
        float normX = (spectrumHoverPos.x - float(graphArea.getX())) / float(graphArea.getWidth());
        float hoverFreq = 20.0f * std::pow(20000.0f / 20.0f, std::clamp(normX, 0.001f, 0.999f));
        float hoverDb = juce::jmap(spectrumHoverPos.y, float(graphArea.getBottom()), float(graphArea.getY()), -28.0f, 18.0f);

        // Musical note calculation
        float noteNum = 69.0f + 12.0f * std::log2(hoverFreq / 440.0f);
        int midiNote = int(std::round(noteNum));
        int cents = int(std::round((noteNum - float(midiNote)) * 100.0f));
        const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int noteIdx = (midiNote % 12 + 12) % 12;
        int octave = (midiNote / 12) - 1;
        juce::String noteStr = juce::String(noteNames[noteIdx]) + juce::String(octave);
        if (cents != 0)
            noteStr += (cents > 0 ? " +" : " ") + juce::String(cents) + "c";

        juce::String freqStr;
        if (hoverFreq >= 1000.0f)
            freqStr = juce::String(hoverFreq / 1000.0f, 2) + " kHz";
        else
            freqStr = juce::String(int(std::round(hoverFreq))) + " Hz";

        // Crosshairs
        const float dashPattern[] = { 3.0f, 3.0f };
        g.setColour(juce::Colour(0x5500f0ff));
        juce::Line<float> vLine(spectrumHoverPos.x, float(graphArea.getY()), spectrumHoverPos.x, float(graphArea.getBottom()));
        g.drawDashedLine(vLine, dashPattern, 2, 1.0f);

        g.setColour(juce::Colour(0x55ffaa00));
        juce::Line<float> hLine(float(graphArea.getX()), spectrumHoverPos.y, float(graphArea.getRight()), spectrumHoverPos.y);
        g.drawDashedLine(hLine, dashPattern, 2, 1.0f);

        // Floating Precision Inspector Badge
        int badgeW = 168;
        int badgeH = 20;
        int badgeX = std::clamp(int(spectrumHoverPos.x) + 12, graphArea.getX() + 4, graphArea.getRight() - badgeW - 4);
        int badgeY = std::clamp(int(spectrumHoverPos.y) - 24, graphArea.getY() + 4, graphArea.getBottom() - badgeH - 4);
        auto badgeRect = juce::Rectangle<float>(float(badgeX), float(badgeY), float(badgeW), float(badgeH));

        g.setColour(juce::Colour(0xf008101a));
        g.fillRoundedRectangle(badgeRect, 4.0f);
        g.setColour(juce::Colour(0xff00f0ff));
        g.drawRoundedRectangle(badgeRect, 4.0f, 1.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font("monospace", 9.5f, juce::Font::bold));
        g.drawText(freqStr + " | " + noteStr + " | " + juce::String(hoverDb, 1) + " dB", badgeRect, juce::Justification::centred);
    }
}

// ------------------------------------------------------------------------------
// Draw Interactive EQ Node Marker
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawEQNode(juce::Graphics& g, float x, float y, int bandNum, float gainDb, juce::Colour col)
{
    // Outer Ring Glow
    g.setColour(col.withAlpha(0.35f));
    g.fillEllipse(x - 9.0f, y - 9.0f, 18.0f, 18.0f);

    // Inner Solid Circle
    g.setColour(juce::Colour(0xff080d14));
    g.fillEllipse(x - 6.5f, y - 6.5f, 13.0f, 13.0f);

    g.setColour(col);
    g.drawEllipse(x - 6.5f, y - 6.5f, 13.0f, 13.0f, 1.8f);

    // Band Number Inside Node
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.drawText(juce::String(bandNum), int(x - 6.0f), int(y - 6.0f), 12, 12, juce::Justification::centred);

    // Floating gain tag pill
    if (std::abs(gainDb) > 0.05f)
    {
        auto tagRect = juce::Rectangle<float>(x - 18.0f, y - 24.0f, 36.0f, 13.0f);
        g.setColour(juce::Colour(0xee080e18));
        g.fillRoundedRectangle(tagRect, 3.0f);
        g.setColour(col);
        g.drawRoundedRectangle(tagRect, 3.0f, 0.8f);
        g.setFont(juce::Font(7.5f, juce::Font::bold));
        juce::String sign = gainDb > 0 ? "+" : "";
        g.drawText(sign + juce::String(gainDb, 1), tagRect, juce::Justification::centred);
    }
}

// ------------------------------------------------------------------------------
// Draw Stereo Goniometer & Phase Screen
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawGoniometerScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(0xff06090e));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.0f);

    auto scopeArea = bounds.reduced(10, 16);
    float cx = float(scopeArea.getCentreX());
    float cy = float(scopeArea.getCentreY()) - 8.0f;
    float r = float(std::min(scopeArea.getWidth(), scopeArea.getHeight())) * 0.44f;

    // Crosshairs
    g.setColour(juce::Colour(0xff131c2a));
    g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);
    g.drawEllipse(cx - r * 0.5f, cy - r * 0.5f, r, r, 0.7f);
    g.drawLine(cx - r, cy, cx + r, cy, 0.7f);
    g.drawLine(cx, cy - r, cx, cy + r, 0.7f);
    // Diagonal 45 deg M/S guides
    g.drawLine(cx - r * 0.7f, cy - r * 0.7f, cx + r * 0.7f, cy + r * 0.7f, 0.5f);
    g.drawLine(cx - r * 0.7f, cy + r * 0.7f, cx + r * 0.7f, cy - r * 0.7f, 0.5f);

    // Lissajous Cloud
    const int numSamples = int(scopeL.size());
    g.setColour(juce::Colour(0xa000ffaa));
    for (int i = 0; i < numSamples; i += 2)
    {
        float l = scopeL[size_t(i)];
        float rSample = scopeR[size_t(i)];

        // Mid/Side rotation: X = Side (L - R), Y = Mid (L + R)
        float side = (l - rSample) * 0.707f;
        float mid = (l + rSample) * 0.707f;

        float px = cx + juce::jlimit(-1.0f, 1.0f, side * 2.0f) * r;
        float py = cy - juce::jlimit(-1.0f, 1.0f, mid * 2.0f) * r;

        g.fillEllipse(px - 1.0f, py - 1.0f, 2.0f, 2.0f);
    }

    // Phase Correlation Bar at Bottom
    auto phaseBar = juce::Rectangle<float>(float(bounds.getX() + 16), float(bounds.getBottom() - 26), float(bounds.getWidth() - 32), 8.0f);
    g.setColour(juce::Colour(0xff0d131b));
    g.fillRoundedRectangle(phaseBar, 3.0f);

    float phaseNorm = juce::jmap(juce::jlimit(-1.0f, 1.0f, curPhase), -1.0f, 1.0f, 0.0f, 1.0f);
    float thumbX = phaseBar.getX() + phaseNorm * phaseBar.getWidth();

    // Center zero mark
    g.setColour(juce::Colour(0xff33445b));
    g.drawLine(phaseBar.getCentreX(), phaseBar.getY() - 2, phaseBar.getCentreX(), phaseBar.getBottom() + 2, 1.0f);

    // Phase thumb indicator
    juce::Colour phaseCol = curPhase >= 0.0f ? juce::Colour(0xff00ffaa) : juce::Colour(0xffff3366);
    g.setColour(phaseCol);
    g.fillRoundedRectangle(thumbX - 4.0f, phaseBar.getY() - 2.0f, 8.0f, 12.0f, 2.0f);

    // Screen Header Tag
    g.setColour(juce::Colour(0xff00ffaa));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("STEREO GONIOMETER & PHASE", bounds.getX() + 10, bounds.getY() + 6, 200, 14, juce::Justification::left);

    g.setFont(juce::Font("monospace", 9.0f, juce::Font::bold));
    g.setColour(phaseCol);
    g.drawText("CORR: " + juce::String(curPhase, 2), bounds.getRight() - 90, bounds.getY() + 6, 80, 14, juce::Justification::right);
}

// ------------------------------------------------------------------------------
// Draw Precision True-Peak & LUFS Meters Screen (Waves Style)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawMetersScreen(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(0xff06090e));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0xff1c2738));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.0f);

    // Screen Header Tag
    g.setColour(juce::Colour(0xff00f0ff));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("PRECISION TRUE-PEAK & LUFS", bounds.getX() + 10, bounds.getY() + 6, 200, 14, juce::Justification::left);

    auto meterArea = bounds.reduced(12, 16);
    meterArea.removeFromTop(10);

    // Left half: Twin dBTP vertical bar meters (L / R)
    const int barW = 16;
    const int meterH = meterArea.getHeight() - 74;
    const int barY = meterArea.getY() + 4;
    const int barLX = meterArea.getX() + 14;
    const int barRX = barLX + barW + 8;

    // Scale from -36 dBTP to +3 dBTP
    auto dbToY = [barY, meterH](float db)
    {
        return juce::jmap(juce::jlimit(-36.0f, 3.0f, db), -36.0f, 3.0f, float(barY + meterH), float(barY));
    };

    // Draw meter backgrounds
    g.setColour(juce::Colour(0xff0d131c));
    g.fillRoundedRectangle(float(barLX), float(barY), float(barW), float(meterH), 3.0f);
    g.fillRoundedRectangle(float(barRX), float(barY), float(barW), float(meterH), 3.0f);

    // Draw active level bars
    float yPeakL = dbToY(dispPeakL);
    float yPeakR = dbToY(dispPeakR);

    juce::ColourGradient grad(juce::Colour(0xffff3366), float(barLX), float(barY),
                              juce::Colour(0xff00ffaa), float(barLX), float(barY + meterH), false);
    grad.addColour(0.3, juce::Colour(0xffffb92d));
    grad.addColour(0.6, juce::Colour(0xff00f0ff));

    // Left Bar
    g.setGradientFill(grad);
    g.fillRoundedRectangle(float(barLX), yPeakL, float(barW), float(barY + meterH) - yPeakL, 3.0f);
    // Right Bar
    g.fillRoundedRectangle(float(barRX), yPeakR, float(barW), float(barY + meterH) - yPeakR, 3.0f);

    // Peak Hold Indicators
    float yHoldL = dbToY(dispPeakHoldL);
    float yHoldR = dbToY(dispPeakHoldR);
    g.setColour(juce::Colours::white);
    g.drawLine(float(barLX), yHoldL, float(barLX + barW), yHoldL, 2.0f);
    g.drawLine(float(barRX), yHoldR, float(barRX + barW), yHoldR, 2.0f);

    // Target -1.0 dBTP Marker line across both meters
    float ySpotifyCeil = dbToY(-1.0f);
    g.setColour(juce::Colour(0xffff3366));
    g.drawLine(float(barLX - 4), ySpotifyCeil, float(barRX + barW + 4), ySpotifyCeil, 1.2f);

    // Scale ticks
    const float scaleDbs[] = { 0.0f, -1.0f, -6.0f, -12.0f, -18.0f, -24.0f };
    for (float db : scaleDbs)
    {
        float y = dbToY(db);
        g.setColour(db == -1.0f ? juce::Colour(0xffff3366) : juce::Colour(0xff475569));
        g.drawLine(float(barRX + barW + 4), y, float(barRX + barW + 10), y, 1.0f);

        g.setFont(juce::Font("monospace", 8.0f, juce::Font::bold));
        g.drawText(juce::String(int(db)), barRX + barW + 12, int(y) - 6, 26, 12, juce::Justification::left);
    }

    // Right half: Digital Readouts (Waves Master Style)
    auto digitalBox = juce::Rectangle<int>(meterArea.getX() + 98, barY + 2, meterArea.getWidth() - 100, meterH);
    g.setColour(juce::Colour(0xff090e15));
    g.fillRoundedRectangle(digitalBox.toFloat(), 5.0f);
    g.setColour(juce::Colour(0xff1a2636));
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
// Draw Hardware Module Cards (Frosted Dark Glass or Clean Platinum Style)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawModuleCard(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title, juce::Colour accentCol)
{
    // Chassis
    if (isDarkMode)
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff101520), float(bounds.getX()), float(bounds.getY()),
                                               juce::Colour(0xff0a0e16), float(bounds.getX()), float(bounds.getBottom()), false));
    }
    else
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffffffff), float(bounds.getX()), float(bounds.getY()),
                                               juce::Colour(0xfff8fafc), float(bounds.getX()), float(bounds.getBottom()), false));
    }
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);

    // Subtle border
    g.setColour(isDarkMode ? juce::Colour(0xff1c2738) : juce::Colour(0xffcbd5e1));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.0f);

    // Header strip
    auto header = bounds.removeFromTop(24);
    g.setColour(isDarkMode ? juce::Colour(0x10ffffff) : juce::Colour(0x08000000));
    g.fillRect(header.reduced(2, 0));

    // Accent line on left of header
    g.setColour(accentCol);
    g.fillRoundedRectangle(float(header.getX() + 6), float(header.getY() + 6), 3.0f, 12.0f, 1.5f);

    g.setColour(isDarkMode ? juce::Colour(0xffcbd5e1) : juce::Colour(0xff1e293b));
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.drawText(title, header.withTrimmedLeft(14), juce::Justification::centredLeft);
}

// ------------------------------------------------------------------------------
// Draw Multi-Agent Live Console & Dynamic Interactive Legend HUD
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::drawAgentDeck(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (isDarkMode)
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff0d121b), float(bounds.getX()), float(bounds.getY()),
                                               juce::Colour(0xff070a0f), float(bounds.getX()), float(bounds.getBottom()), false));
    }
    else
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffffffff), float(bounds.getX()), float(bounds.getY()),
                                               juce::Colour(0xfff1f5f9), float(bounds.getX()), float(bounds.getBottom()), false));
    }
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);
    g.setColour(isDarkMode ? juce::Colour(0xff1c2738) : juce::Colour(0xffcbd5e1));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.0f);

    // Header with Agent Chips
    auto chipArea = bounds.removeFromTop(32).reduced(10, 4);

    const char* agentNames[] = { "Orchestrator v3.4", "DSP Architect", "True-Peak Guard", "Spotify QA Engine" };
    const juce::Colour agentColors[] = { juce::Colour(0xff00f0ff), juce::Colour(0xff5bc0de), juce::Colour(0xffffb92d), juce::Colour(0xff00ffaa) };

    for (int i = 0; i < 4; ++i)
    {
        int chipX = chipArea.getX() + 12 + i * 160;
        auto chip = juce::Rectangle<float>(float(chipX), float(chipArea.getY() + 4), 150, 18);

        g.setColour(isDarkMode ? juce::Colour(0xff121b27) : juce::Colour(0xfff1f5f9));
        g.fillRoundedRectangle(chip, 9.0f);
        g.setColour(isDarkMode ? juce::Colour(0xff1e2d42) : juce::Colour(0xffcbd5e1));
        g.drawRoundedRectangle(chip, 9.0f, 0.8f);

        // Active pulse dot
        g.setColour(agentColors[i]);
        g.fillEllipse(float(chipX + 8), float(chipArea.getY() + 9), 8.0f, 8.0f);

        g.setColour(isDarkMode ? juce::Colours::white : juce::Colour(0xff0f172a));
        g.setFont(juce::Font(9.0f, juce::Font::bold));
        g.drawText(agentNames[i], chipX + 22, chipArea.getY() + 4, 120, 18, juce::Justification::centredLeft);
    }

    g.setColour(isDarkMode ? juce::Colour(0xff00ffaa) : juce::Colour(0xff059669));
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.drawText("AI COOPERATIVE CORE v3.4 | BY VEVI", chipArea.getRight() - 260, chipArea.getY(), 250, 24, juce::Justification::right);

    // Terminal Window / Interactive Legend HUD
    auto termArea = bounds.reduced(10, 8);
    g.setColour(isDarkMode ? juce::Colour(0xff040609) : juce::Colour(0xff0f172a));
    g.fillRoundedRectangle(termArea.toFloat(), 5.0f);
    g.setColour(isDarkMode ? juce::Colour(0xff1c2738) : juce::Colour(0xff334155));
    g.drawRoundedRectangle(termArea.toFloat(), 5.0f, 1.0f);

    if (isControlActive)
    {
        // Active Control Glowing Accent Border
        g.setColour(activeLegend.accent.withAlpha(0.6f));
        g.drawRoundedRectangle(termArea.toFloat(), 5.0f, 1.2f);

        // Line 1: Control Identification & Current Value
        g.setFont(juce::Font("monospace", 11.0f, juce::Font::bold));
        g.setColour(activeLegend.accent);
        g.drawText("[ACTIVE CONTROL] " + activeLegend.name, termArea.getX() + 10, termArea.getY() + 4, 380, 16, juce::Justification::left);

        g.setColour(juce::Colour(0xff94a3b8));
        g.drawText("MODULE: " + activeLegend.moduleName + "  |  RANGE: " + activeLegend.rangeStr, termArea.getX() + 390, termArea.getY() + 4, 460, 16, juce::Justification::left);

        g.setColour(juce::Colour(0xff00ffaa));
        g.drawText("LIVE: " + activeControlValue, termArea.getRight() - 200, termArea.getY() + 4, 190, 16, juce::Justification::right);

        // Line 2: Function Description
        g.setFont(juce::Font(10.5f, juce::Font::plain));
        g.setColour(juce::Colour(0xffffb92d));
        g.drawText("FUNCTION:", termArea.getX() + 10, termArea.getY() + 24, 75, 16, juce::Justification::left);
        g.setColour(juce::Colours::white);
        g.drawText(activeLegend.description, termArea.getX() + 90, termArea.getY() + 24, termArea.getWidth() - 100, 16, juce::Justification::left);

        // Line 3: Spotify Target Rule
        g.setColour(juce::Colour(0xff00ffaa));
        g.drawText("SPOTIFY TARGET:", termArea.getX() + 10, termArea.getY() + 44, 115, 16, juce::Justification::left);
        g.setColour(juce::Colour(0xffcbd5e1));
        g.drawText(activeLegend.spotifyTip, termArea.getX() + 130, termArea.getY() + 44, termArea.getWidth() - 340, 16, juce::Justification::left);

        // Interaction shortcut
        g.setColour(juce::Colour(0xff64748b));
        g.drawText("TIP: Vertical drag / mouse wheel | Double-click to reset", termArea.getRight() - 340, termArea.getY() + 44, 330, 16, juce::Justification::right);
    }
    else
    {
        // Idle Mode: Legend Prompt + AI Telemetry
        g.setFont(juce::Font("monospace", 10.5f, juce::Font::plain));
        g.setColour(juce::Colour(0xff00f0ff));
        g.drawText("[INTERACTIVE LEGEND v3.4] Hover or tweak any control to inspect function, Spotify target, and mastering tips.", termArea.getX() + 10, termArea.getY() + 6, termArea.getWidth() - 20, 16, juce::Justification::left);

        g.setColour(juce::Colour(0xff5bc0de));
        g.drawText("[DSP Architect] 5-band dynamic curves calibrated to Spotify (-14.0 LUFS Integrated / -1.0 dBTP Ceiling)", termArea.getX() + 10, termArea.getY() + 24, termArea.getWidth() - 20, 16, juce::Justification::left);

        g.setColour(juce::Colour(0xffffb92d));
        g.drawText("[True-Peak Guard] Inter-sample peak limiter active | Mono-Maker (<110Hz) locked for tight sub bass", termArea.getX() + 10, termArea.getY() + 42, termArea.getWidth() - 20, 16, juce::Justification::left);

        g.setColour(juce::Colour(0xff00ffaa));
        g.drawText("[Global Status] CERTIFIED FOR SPOTIFY MASTERING | Click 'CONTROL GUIDE' above for full module manual", termArea.getX() + 10, termArea.getY() + 60, termArea.getWidth() - 20, 16, juce::Justification::left);
    }
}

// ------------------------------------------------------------------------------
// Layout Coordinates for Knobs and Sliders (Window 1296 x 888)
// ------------------------------------------------------------------------------
void AutomasterSupremeAudioProcessorEditor::resized()
{
    // Top Bar controls (Width: 1296) - ZERO OVERLAPS
    const int topBarY = 17;
    const int topBarH = 34;

    // AI Auto Master on far right
    const int ambW = 145;
    const int ambX = getWidth() - 20 - ambW; // 1296 - 20 - 145 = 1131
    autoMasterButton.setBounds(ambX, topBarY, ambW, topBarH);

    // Control Guide Button
    const int legW = 140;
    const int legX = ambX - 10 - legW; // 1131 - 10 - 140 = 981
    legendButton.setBounds(legX, topBarY, legW, topBarH);

    // Theme Mode Toggle Button
    const int thmW = 125;
    const int thmX = legX - 10 - thmW; // 981 - 10 - 125 = 846
    themeButton.setBounds(thmX, topBarY, thmW, topBarH);

    // Preset ComboBox (Fits full name comfortably)
    const int pbW = 330;
    const int pbX = thmX - 12 - pbW; // 846 - 12 - 330 = 504
    presetBox.setBounds(pbX, topBarY, pbW, topBarH);
    // Note: Left side spotifyTag ends at 476 -> 28px clean gap before presetBox!

    // Close button inside full legend drawer overlay
    closeLegendButton.setBounds(getWidth() - 230, 48, 160, 30);

    // Reset Peaks Button (Inside SPAN Spectrum Header)
    resetPeaksButton.setBounds(548, 88, 104, 18);

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
