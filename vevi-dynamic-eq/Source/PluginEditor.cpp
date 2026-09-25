#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// ProQLookAndFeel Implementation
// ==============================================================================
ProQLookAndFeel::ProQLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff34d399));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff1c212e));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xff34d399));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff121722));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff222a3d));
    setColour(juce::ComboBox::textColourId, juce::Colours::white);
    setColour(juce::ToggleButton::textColourId, juce::Colours::white);
}

void ProQLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                          const juce::Colour& backgroundColour,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
    auto baseCol = backgroundColour;

    if (shouldDrawButtonAsDown)
        baseCol = baseCol.brighter(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseCol = baseCol.brighter(0.1f);

    g.setColour(baseCol);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(baseCol.brighter(0.15f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

void ProQLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPosProportional, float rotaryStartAngle,
                                       float rotaryEndAngle, juce::Slider& slider)
{
    auto radius = (float)juce::jmin(width / 2, height / 2) - 3.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto activeColour = slider.findColour(juce::Slider::rotarySliderFillColourId);

    // Deep Metallic Charcoal Dial Base
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff1c2432), centreX, ry,
                                           juce::Colour(0xff0d1117), centreX, ry + rw, false));
    g.fillEllipse(rx, ry, rw, rw);

    // Outer subtle border
    g.setColour(juce::Colour(0xff273247));
    g.drawEllipse(rx, ry, rw, rw, 1.2f);

    // Background track arc
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius - 4.5f, radius - 4.5f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff151b26));
    g.strokePath(backgroundArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active Value Arc with Glowing Neon
    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius - 4.5f, radius - 4.5f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(activeColour.withAlpha(0.28f));
    g.strokePath(valueArc, juce::PathStrokeType(7.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    g.setColour(activeColour);
    g.strokePath(valueArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Inner dial cap
    float capRadius = radius - 8.5f;
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff222b3b), centreX, centreY - capRadius,
                                           juce::Colour(0xff101520), centreX, centreY + capRadius, false));
    g.fillEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    g.setColour(juce::Colour(0x28ffffff));
    g.drawEllipse(centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.0f);

    // Indicator needle dot
    juce::Point<float> needle(centreX + (capRadius - 3.5f) * std::cos(angle - juce::MathConstants<float>::halfPi),
                              centreY + (capRadius - 3.5f) * std::sin(angle - juce::MathConstants<float>::halfPi));
    g.setColour(activeColour);
    g.fillEllipse(needle.x - 2.8f, needle.y - 2.8f, 5.6f, 5.6f);
    g.setColour(juce::Colours::white);
    g.fillEllipse(needle.x - 1.4f, needle.y - 1.4f, 2.8f, 2.8f);
}

// ==============================================================================
// ProQPlotComponent Implementation
// ==============================================================================
ProQPlotComponent::ProQPlotComponent(VeviProQAudioProcessor& p)
    : processor(p)
{
    smoothedPreScope.resize(VeviProQAudioProcessor::FFT_SIZE / 2, 0.0f);
    smoothedPostScope.resize(VeviProQAudioProcessor::FFT_SIZE / 2, 0.0f);
    startTimerHz(45);
}

ProQPlotComponent::~ProQPlotComponent()
{
    stopTimer();
}

void ProQPlotComponent::setSelectedBand(int idx)
{
    if (idx >= 0 && idx < VeviProQAudioProcessor::NUM_BANDS)
    {
        selectedBand = idx;
        repaint();
        if (onBandSelected)
            onBandSelected(selectedBand);
    }
}

void ProQPlotComponent::timerCallback()
{
    if (processor.getNextAudioBlockReady())
    {
        // 1. Process Pre-EQ (Raw Input) FFT
        auto preData = processor.getPreFFTData();
        processor.getWindow().multiplyWithWindowingTable(preData.data(), VeviProQAudioProcessor::FFT_SIZE);
        processor.getForwardFFT().performFrequencyOnlyForwardTransform(preData.data());

        // 2. Process Post-EQ (Processed Output) FFT
        auto postData = processor.getPostFFTData();
        processor.getWindow().multiplyWithWindowingTable(postData.data(), VeviProQAudioProcessor::FFT_SIZE);
        processor.getForwardFFT().performFrequencyOnlyForwardTransform(postData.data());

        const size_t numBins = smoothedPostScope.size();
        const float binWidth = 44100.0f / (float)VeviProQAudioProcessor::FFT_SIZE;

        for (size_t i = 1; i < numBins; ++i)
        {
            float f = (float)i * binWidth;
            float tiltGain = 1.0f;
            if (f > 20.0f)
            {
                float octavesFrom1k = std::log2(f / 1000.0f);
                float tiltDB = octavesFrom1k * 3.5f;
                tiltGain = juce::Decibels::decibelsToGain(tiltDB);
            }

            // Pre-EQ level calculation
            float rawPre = preData[i] / (float)VeviProQAudioProcessor::FFT_SIZE;
            float preLevelDB = juce::Decibels::gainToDecibels(rawPre * tiltGain, -80.0f);
            float preNorm01 = juce::jlimit(0.0f, 1.0f, juce::jmap(preLevelDB, -66.0f, 0.0f, 0.0f, 1.0f));

            if (preNorm01 > smoothedPreScope[i])
                smoothedPreScope[i] = smoothedPreScope[i] * 0.40f + preNorm01 * 0.60f;
            else
                smoothedPreScope[i] = smoothedPreScope[i] * 0.82f + preNorm01 * 0.18f;

            // Post-EQ level calculation
            float rawPost = postData[i] / (float)VeviProQAudioProcessor::FFT_SIZE;
            float postLevelDB = juce::Decibels::gainToDecibels(rawPost * tiltGain, -80.0f);
            float postNorm01 = juce::jlimit(0.0f, 1.0f, juce::jmap(postLevelDB, -66.0f, 0.0f, 0.0f, 1.0f));

            if (postNorm01 > smoothedPostScope[i])
                smoothedPostScope[i] = smoothedPostScope[i] * 0.40f + postNorm01 * 0.60f;
            else
                smoothedPostScope[i] = smoothedPostScope[i] * 0.82f + postNorm01 * 0.18f;
        }
    }
    repaint();
}

float ProQPlotComponent::getXForFrequency(float freq) const
{
    const float w = (float)getWidth();
    return w * (std::log10(freq / 20.0f) / std::log10(20000.0f / 20.0f));
}

float ProQPlotComponent::getFrequencyForX(float x) const
{
    const float w = (float)getWidth();
    const float prop = juce::jlimit(0.0f, 1.0f, x / w);
    return 20.0f * std::pow(20000.0f / 20.0f, prop);
}

float ProQPlotComponent::getYForGain(float gainDB) const
{
    const float h = (float)getHeight();
    const float limit = (currentDbScale > 0.0f) ? currentDbScale : 12.0f;
    return juce::jmap(gainDB, limit, -limit, 15.0f, h - 25.0f);
}

float ProQPlotComponent::getGainForY(float y) const
{
    const float h = (float)getHeight();
    const float limit = (currentDbScale > 0.0f) ? currentDbScale : 12.0f;
    return juce::jmap(y, 15.0f, h - 25.0f, limit, -limit);
}

juce::Point<float> ProQPlotComponent::getBandNodePos(int bandIdx) const
{
    auto s = processor.getBandSettings(bandIdx);
    return { getXForFrequency(s.frequency), getYForGain(s.gainDB) };
}

juce::String ProQPlotComponent::getNoteNameForFreq(float freq)
{
    if (freq <= 0) return "--";
    const double pitch = 69.0 + 12.0 * std::log2(freq / 440.0);
    const int midiNote = (int)std::round(pitch);
    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (midiNote / 12) - 1;
    int noteIdx = ((midiNote % 12) + 12) % 12;
    return juce::String(noteNames[noteIdx]) + juce::String(octave);
}

void ProQPlotComponent::drawPianoRoll(juce::Graphics& g, float yPos, float height)
{
    const float w = (float)getWidth();

    g.setColour(juce::Colour(0xff090c12));
    g.fillRect(0.0f, yPos, w, height);

    g.setColour(juce::Colour(0xff161d2a));
    g.drawHorizontalLine((int)yPos, 0.0f, w);
    g.drawHorizontalLine((int)(yPos + height), 0.0f, w);

    static const struct { const char* name; float freq; } notes[] = {
        { "C1", 32.70f },  { "C2", 65.41f },  { "C3", 130.81f },
        { "C4", 261.63f }, { "A4", 440.0f },  { "C5", 523.25f },
        { "C6", 1046.5f }, { "C7", 2093.0f }, { "C8", 4186.0f }
    };

    g.setFont(juce::Font(9.5f, juce::Font::bold));

    for (const auto& n : notes)
    {
        float x = getXForFrequency(n.freq);
        if (x < 0 || x > w) continue;

        if (n.freq == 440.0f)
        {
            g.setColour(juce::Colour(0xffa855f7).withAlpha(0.25f));
            g.fillRect(x - 10.0f, yPos, 20.0f, height);
            g.setColour(juce::Colour(0xffc084fc));
        }
        else if (n.freq >= 1000.0f && n.freq <= 1500.0f)
        {
            g.setColour(juce::Colour(0xff06b6d4).withAlpha(0.2f));
            g.fillRect(x - 8.0f, yPos, 16.0f, height);
            g.setColour(juce::Colour(0xff38bdf8));
        }
        else
        {
            g.setColour(juce::Colour(0xff222a3a));
            g.drawVerticalLine((int)x, yPos, yPos + height);
            g.setColour(juce::Colour(0xff64748b));
        }

        g.drawText(n.name, (int)x - 14, (int)yPos + 1, 28, (int)height - 2, juce::Justification::centred);
    }
}

void ProQPlotComponent::paint(juce::Graphics& g)
{
    const float w = (float)getWidth();
    const float h = (float)getHeight();

    // Deep Dark Navy Background
    g.fillAll(juce::Colour(0xff07090e));

    // Frequency Vertical Grid Lines
    const float freqs[] = { 30.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
    g.setFont(juce::Font(10.0f));

    for (float f : freqs)
    {
        float x = getXForFrequency(f);
        g.setColour(juce::Colour(0x15ffffff));
        g.drawVerticalLine((int)x, 0.0f, h - 22.0f);

        g.setColour(juce::Colour(0x5594a3b8));
        juce::String label = (f >= 1000.0f) ? juce::String((int)(f / 1000.0f)) + "k" : juce::String((int)f);
        g.drawText(label, (int)x - 16, (int)h - 38, 32, 14, juce::Justification::centred);
    }

    // Gain dB Horizontal Grid Lines (EQ Response Scale)
    const float limit = (currentDbScale > 0.0f) ? currentDbScale : 12.0f;
    std::vector<float> gains;
    if (limit <= 3.5f)
        gains = { 3.0f, 2.0f, 1.0f, 0.0f, -1.0f, -2.0f, -3.0f };
    else if (limit <= 6.5f)
        gains = { 6.0f, 4.0f, 2.0f, 0.0f, -2.0f, -4.0f, -6.0f };
    else if (limit <= 15.0f)
        gains = { 12.0f, 9.0f, 6.0f, 3.0f, 0.0f, -3.0f, -6.0f, -9.0f, -12.0f };
    else
        gains = { 30.0f, 20.0f, 10.0f, 0.0f, -10.0f, -20.0f, -30.0f };

    for (float db : gains)
    {
        float y = getYForGain(db);

        if (std::abs(db) < 0.1f)
        {
            // 0 dB center reference line (dashed cyan)
            g.setColour(juce::Colour(0xff38bdf8).withAlpha(0.6f));
            float dashPattern[2] = { 4.0f, 4.0f };
            g.drawDashedLine(juce::Line<float>(0.0f, y, w, y), dashPattern, 2, 1.0f);

            g.setColour(juce::Colour(0xff38bdf8));
            g.drawText("0 dB", (int)w - 38, (int)y - 7, 34, 14, juce::Justification::right);
            g.drawText("0 dB", 4, (int)y - 7, 34, 14, juce::Justification::left);
        }
        else
        {
            g.setColour(juce::Colour(0x12ffffff));
            g.drawHorizontalLine((int)y, 0.0f, w);

            g.setColour(juce::Colour(0x4494a3b8));
            juce::String txt = (db > 0 ? "+" : "") + juce::String((int)db);
            g.drawText(txt, (int)w - 28, (int)y - 7, 24, 14, juce::Justification::right);
            g.drawText(txt, 4, (int)y - 7, 24, 14, juce::Justification::left);
        }
    }

    // --- 1. RTA DUAL SPECTRUM ANALYZER (PRE-EQ RAW vs POST-EQ PROCESSED) ---
    if (isRtaAnalyzerVisible)
    {
        const float bottomY = h - 22.0f;
        const size_t numBins = smoothedPostScope.size();
        const float binWidth = 44100.0f / (float)VeviProQAudioProcessor::FFT_SIZE;

        // Lambda to sample any scope buffer (Pre or Post) at continuous frequency with Catmull-Rom cubic interpolation
        auto sampleSpectrumBuffer = [&](const std::vector<float>& buffer, float freq) -> float
        {
            if (freq < 20.0f || freq > 20000.0f || buffer.empty()) return 0.0f;
            float binPos = freq / binWidth;
            int idx0 = juce::jlimit(1, (int)numBins - 1, (int)std::floor(binPos));
            int idx1 = juce::jlimit(1, (int)numBins - 1, idx0 + 1);
            int idxPrev = juce::jlimit(1, (int)numBins - 1, idx0 - 1);
            int idxNext = juce::jlimit(1, (int)numBins - 1, idx1 + 1);

            float frac = binPos - (float)idx0;

            float p0 = buffer[(size_t)idxPrev];
            float p1 = buffer[(size_t)idx0];
            float p2 = buffer[(size_t)idx1];
            float p3 = buffer[(size_t)idxNext];

            if (binPos > 12.0f)
            {
                int span = (int)std::ceil(binPos * 0.035f);
                float maxVal = p1;
                int startB = std::max(1, idx0 - span);
                int endB = std::min((int)numBins - 1, idx0 + span);
                for (int b = startB; b <= endB; ++b)
                    maxVal = std::max(maxVal, buffer[(size_t)b]);
                return maxVal;
            }

            float c = 0.5f * (
                (2.0f * p1) +
                (-p0 + p2) * frac +
                (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * (frac * frac) +
                (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * (frac * frac * frac)
            );
            return juce::jlimit(0.0f, 1.0f, c);
        };

        const int stepPx = 3;

        // --- LAYER A: PRE-EQ (RAW INPUT SPECTRUM - Translucent Slate/Cyan Outline) ---
        juce::Path prePath;
        prePath.startNewSubPath(0.0f, bottomY);
        for (int px = 0; px <= (int)w; px += stepPx)
        {
            float f = getFrequencyForX((float)px);
            float mag = sampleSpectrumBuffer(smoothedPreScope, f);
            float y = bottomY - (mag * (bottomY - 20.0f));
            prePath.lineTo((float)px, y);
        }
        prePath.lineTo(w, bottomY);
        prePath.closeSubPath();

        // Fill Pre-EQ with soft moody charcoal/cyan gradient
        g.setGradientFill(juce::ColourGradient(juce::Colour(0x3538bdf8), 0.0f, h * 0.45f,
                                               juce::Colour(0x0238bdf8), 0.0f, bottomY, false));
        g.fillPath(prePath);

        // Pre-EQ boundary dashed/subtle stroke
        g.setColour(juce::Colour(0x6538bdf8));
        float preDash[2] = { 4.0f, 3.0f };
        g.drawDashedLine(juce::Line<float>(0.0f, bottomY, 0.0f, bottomY), preDash, 2, 1.0f);
        g.strokePath(prePath, juce::PathStrokeType(1.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // --- LAYER B: POST-EQ (PROCESSED / EQUALIZED SPECTRUM - Glowing Warm Amber) ---
        juce::Path postPath;
        postPath.startNewSubPath(0.0f, bottomY);
        for (int px = 0; px <= (int)w; px += stepPx)
        {
            float f = getFrequencyForX((float)px);
            float mag = sampleSpectrumBuffer(smoothedPostScope, f);
            float y = bottomY - (mag * (bottomY - 20.0f));
            postPath.lineTo((float)px, y);
        }
        postPath.lineTo(w, bottomY);
        postPath.closeSubPath();

        // Post-EQ warm amber fill
        g.setGradientFill(juce::ColourGradient(juce::Colour(0x55f59e0b), 0.0f, h * 0.35f,
                                               juce::Colour(0x04f59e0b), 0.0f, bottomY, false));
        g.fillPath(postPath);

        // Glowing outer neon stroke for Post-EQ
        g.setColour(juce::Colour(0xfff59e0b).withAlpha(0.28f));
        g.strokePath(postPath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour(juce::Colour(0xfffbbf24));
        g.strokePath(postPath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // --- 2. DYNAMIC SHADING AREA & INDIVIDUAL BAND CURVES ---
    for (int i = 0; i < VeviProQAudioProcessor::NUM_BANDS; ++i)
    {
        auto s = processor.getBandSettings(i);
        if (!s.enabled) continue;

        auto bandCol = getBandColour(i);
        bool isSelected = (i == selectedBand);

        juce::Path bandPath;
        for (int px = 0; px < (int)w; px += 2)
        {
            float f = getFrequencyForX((float)px);
            double mag = processor.getBandMagnitudeForFrequency(i, f);
            float magDB = (float)juce::Decibels::gainToDecibels(mag);
            float y = getYForGain(magDB);

            if (px == 0) bandPath.startNewSubPath(0.0f, y);
            else bandPath.lineTo((float)px, y);
        }

        g.setColour(bandCol.withAlpha(isSelected ? 0.35f : 0.12f));
        g.strokePath(bandPath, juce::PathStrokeType(isSelected ? 1.6f : 1.0f));

        // Dynamic Shading Area
        if (s.dynamicEnabled && std::abs(s.dynamicRangeDB) > 0.05f)
        {
            juce::Path dynShade;
            for (int px = 0; px < (int)w; px += 2)
            {
                float f = getFrequencyForX((float)px);
                double mag = processor.getBandMagnitudeWithGain(i, f, s.gainDB);
                float y = getYForGain((float)juce::Decibels::gainToDecibels(mag));
                if (px == 0) dynShade.startNewSubPath(0.0f, y);
                else dynShade.lineTo((float)px, y);
            }
            for (int px = (int)w - 1; px >= 0; px -= 2)
            {
                float f = getFrequencyForX((float)px);
                double mag = processor.getBandMagnitudeWithGain(i, f, s.gainDB + s.dynamicRangeDB);
                float y = getYForGain((float)juce::Decibels::gainToDecibels(mag));
                dynShade.lineTo((float)px, y);
            }
            dynShade.closeSubPath();

            juce::Colour dynTint = (s.dynamicRangeDB >= 0.0f) ? juce::Colour(0xff34d399) : juce::Colour(0xfff43f5e);
            g.setColour(dynTint.withAlpha(0.22f));
            g.fillPath(dynShade);

            // Active dynamic real-time pulse
            float liveDynGain = processor.getBandDynamicGain(i);
            if (std::abs(liveDynGain) > 0.05f)
            {
                juce::Path dynLive;
                for (int px = 0; px < (int)w; px += 2)
                {
                    float f = getFrequencyForX((float)px);
                    double mag = processor.getBandMagnitudeWithGain(i, f, s.gainDB + liveDynGain);
                    float y = getYForGain((float)juce::Decibels::gainToDecibels(mag));
                    if (px == 0) dynLive.startNewSubPath(0.0f, y);
                    else dynLive.lineTo((float)px, y);
                }
                g.setColour(dynTint.withAlpha(0.7f));
                g.strokePath(dynLive, juce::PathStrokeType(1.6f));
            }
        }
    }

    // --- 3. TOTAL SUMMED MASTER EQ CURVE (Glowing Aqua Neon) ---
    juce::Path masterCurve;
    for (int px = 0; px < (int)w; px += 2)
    {
        float f = getFrequencyForX((float)px);
        double mag = processor.getMagnitudeForFrequency(f);
        float y = getYForGain((float)juce::Decibels::gainToDecibels(mag));

        if (px == 0) masterCurve.startNewSubPath(0.0f, y);
        else masterCurve.lineTo((float)px, y);
    }

    // Fill under master curve
    juce::Path fillCurve = masterCurve;
    fillCurve.lineTo(w, h - 22.0f);
    fillCurve.lineTo(0.0f, h - 22.0f);
    fillCurve.closeSubPath();

    g.setGradientFill(juce::ColourGradient(juce::Colour(0x2838bdf8), 0.0f, 0.0f,
                                           juce::Colour(0x0238bdf8), 0.0f, h - 22.0f, false));
    g.fillPath(fillCurve);

    // Glowing Neon Stroke
    g.setColour(juce::Colour(0xff38bdf8).withAlpha(0.35f));
    g.strokePath(masterCurve, juce::PathStrokeType(5.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour(juce::Colour(0xff38bdf8));
    g.strokePath(masterCurve, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // --- 4. PIANO ROLL REFERENCE STRIP AT BOTTOM ---
    if (isPianoRollVisible)
    {
        drawPianoRoll(g, h - 20.0f, 20.0f);
    }

    // --- 5. BAND NODES (1 TO 7) WITH FABFILTER GLOW & DYNAMIC RINGS ---
    for (int i = 0; i < VeviProQAudioProcessor::NUM_BANDS; ++i)
    {
        auto s = processor.getBandSettings(i);
        if (!s.enabled) continue;

        auto node = getBandNodePos(i);
        bool isSelected = (i == selectedBand);
        auto col = getBandColour(i);

        // Dynamic concentric dashed ring for dynamic bands
        if (s.dynamicEnabled)
        {
            g.setColour(col.withAlpha(0.6f));
            float dashPattern[2] = { 3.0f, 3.0f };
            g.drawDashedLine(juce::Line<float>(node.x - 14.0f, node.y, node.x + 14.0f, node.y), dashPattern, 2, 1.0f);

            juce::Path halo;
            halo.addCentredArc(node.x, node.y, 16.0f, 16.0f, 0.0f, 0.0f, juce::MathConstants<float>::twoPi, true);
            g.setColour(col.withAlpha(0.75f));
            g.strokePath(halo, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::square));
        }

        // Selected halo
        if (isSelected)
        {
            g.setColour(col.withAlpha(0.35f));
            g.fillEllipse(node.x - 18.0f, node.y - 18.0f, 36.0f, 36.0f);
            g.setColour(col);
            g.drawEllipse(node.x - 18.0f, node.y - 18.0f, 36.0f, 36.0f, 1.5f);
        }

        // Inner solid pill
        g.setColour(col);
        g.fillEllipse(node.x - 9.0f, node.y - 9.0f, 18.0f, 18.0f);
        g.setColour(juce::Colour(0xff090c12));
        g.drawEllipse(node.x - 9.0f, node.y - 9.0f, 18.0f, 18.0f, 1.5f);

        // Number label
        g.setColour(juce::Colour(0xff090c12));
        g.setFont(juce::Font(10.5f, juce::Font::bold));
        g.drawText(juce::String(i + 1), (int)node.x - 9, (int)node.y - 9, 18, 18, juce::Justification::centred);
    }
}

void ProQPlotComponent::resized() {}

void ProQPlotComponent::mouseDown(const juce::MouseEvent& e)
{
    draggedBand = -1;
    auto mousePos = e.position;

    for (int i = 0; i < VeviProQAudioProcessor::NUM_BANDS; ++i)
    {
        auto node = getBandNodePos(i);
        if (mousePos.getDistanceFrom(node) <= 18.0f)
        {
            selectedBand = i;
            draggedBand = i;
            if (onBandSelected)
                onBandSelected(selectedBand);
            repaint();
            return;
        }
    }
}

void ProQPlotComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (draggedBand >= 0 && draggedBand < VeviProQAudioProcessor::NUM_BANDS)
    {
        float newFreq = getFrequencyForX(e.position.x);
        float newGain = getGainForY(e.position.y);

        auto* freqParam = processor.apvts.getParameter(VeviProQAudioProcessor::getParamId(draggedBand, "freq"));
        auto* gainParam = processor.apvts.getParameter(VeviProQAudioProcessor::getParamId(draggedBand, "gain"));

        if (freqParam != nullptr)
            freqParam->setValueNotifyingHost(freqParam->convertTo0to1(newFreq));

        if (gainParam != nullptr)
            gainParam->setValueNotifyingHost(gainParam->convertTo0to1(newGain));

        if (onBandDragged)
            onBandDragged(draggedBand);

        repaint();
    }
}

void ProQPlotComponent::mouseUp(const juce::MouseEvent&)
{
    draggedBand = -1;
}

void ProQPlotComponent::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    if (selectedBand >= 0 && selectedBand < VeviProQAudioProcessor::NUM_BANDS)
    {
        auto* qParam = processor.apvts.getParameter(VeviProQAudioProcessor::getParamId(selectedBand, "q"));
        if (qParam != nullptr)
        {
            float currentQ = qParam->convertFrom0to1(qParam->getValue());
            float delta = wheel.deltaY * 0.4f;
            float newQ = std::clamp(currentQ + delta, 0.1f, 30.0f);
            qParam->setValueNotifyingHost(qParam->convertTo0to1(newQ));
            repaint();
        }
    }
}

// ==============================================================================
// VeviProQAudioProcessorEditor Main Implementation
// ==============================================================================
VeviProQAudioProcessorEditor::VeviProQAudioProcessorEditor (VeviProQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), plotComponent (p)
{
    setLookAndFeel (&lnf);
    // 25% larger fixed resolution requested by user: 1375 x 850
    setSize (1375, 850);
    setResizable (false, false);

    auto setupBtn = [this](juce::TextButton& b, juce::Colour bg, juce::Colour text) {
        b.setColour(juce::TextButton::buttonColourId, bg);
        b.setColour(juce::TextButton::textColourOffId, text);
        addAndMakeVisible(b);
    };

    setupBtn(btnAiMaster, juce::Colour(0xff182030), juce::Colour(0xff38bdf8));
    setupBtn(btnPower, juce::Colour(0xff182030), juce::Colour(0xff34d399));
    setupBtn(btnFullscreen, juce::Colour(0xff182030), juce::Colour(0xff94a3b8));

    btnPower.onClick = [this]() {
        auto* p = audioProcessor.apvts.getParameter("global_bypass");
        if (p)
        {
            bool bypass = p->getValue() > 0.5f;
            p->setValueNotifyingHost(bypass ? 0.0f : 1.0f);
            btnPower.setColour(juce::TextButton::textColourOffId, bypass ? juce::Colour(0xff34d399) : juce::Colour(0xffef4444));
        }
    };

    // Preset navigation setup
    setupBtn(btnPrevPreset, juce::Colour(0xff121722), juce::Colour(0xff94a3b8));
    setupBtn(btnPresetName, juce::Colour(0xff121722), juce::Colour(0xff38bdf8));
    setupBtn(btnNextPreset, juce::Colour(0xff121722), juce::Colour(0xff94a3b8));
    setupBtn(btnABCompare, juce::Colour(0xff121722), juce::Colour(0xff38bdf8));

    btnPrevPreset.onClick = [this]() {
        int cur = audioProcessor.getCurrentProgram();
        int total = audioProcessor.getNumPrograms();
        int next = (cur - 1 + total) % total;
        audioProcessor.setCurrentProgram(next);
        refreshPresetsUI();
    };

    btnNextPreset.onClick = [this]() {
        int cur = audioProcessor.getCurrentProgram();
        int total = audioProcessor.getNumPrograms();
        int next = (cur + 1) % total;
        audioProcessor.setCurrentProgram(next);
        refreshPresetsUI();
    };

    btnPresetName.onClick = [this]() {
        juce::PopupMenu m;
        const auto& presets = VeviProQAudioProcessor::getArtistPresets();
        for (int i = 0; i < (int)presets.size(); ++i)
        {
            m.addItem(i + 1, presets[(size_t)i].name + "  [" + presets[(size_t)i].category + "]", true, i == audioProcessor.getCurrentProgram());
        }
        m.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&btnPresetName), [this](int result) {
            if (result > 0)
            {
                audioProcessor.setCurrentProgram(result - 1);
                refreshPresetsUI();
            }
        });
    };

    btnABCompare.onClick = [this]() {
        static bool isA = true;
        isA = !isA;
        btnABCompare.setButtonText(isA ? "A/B: A" : "A/B: B");
        btnABCompare.setColour(juce::TextButton::textColourOffId, isA ? juce::Colour(0xff38bdf8) : juce::Colour(0xfffbbf24));
    };

    btnAiMaster.onClick = [this]() {
        // AI Master intelligent automatic polish profile
        audioProcessor.loadPreset(1);
        refreshPresetsUI();
    };

    setupBtn(btnTestAudio, juce::Colour(0xff059669), juce::Colours::white);
    btnTestAudio.onClick = [this]() {
        static bool isAuditioning = false;
        isAuditioning = !isAuditioning;
        btnTestAudio.setButtonText(isAuditioning ? "STOP" : "AUDITION");
        btnTestAudio.setColour(juce::TextButton::buttonColourId, isAuditioning ? juce::Colour(0xffef4444) : juce::Colour(0xff059669));
    };

    sampleBeatCombo.addItemList({ "FL Beat & 808 Loop (124 BPM)", "Lead Vocal Polish (130 BPM)", "Master Acoustic Drums" }, 1);
    sampleBeatCombo.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(sampleBeatCombo);

    // Subheader Mode bar
    setupBtn(btnPhaseZero, juce::Colour(0xff101520), juce::Colour(0xff94a3b8));
    setupBtn(btnPhaseNatural, juce::Colour(0xff064e3b), juce::Colour(0xff34d399));
    setupBtn(btnPhaseLinear, juce::Colour(0xff101520), juce::Colour(0xff94a3b8));

    btnPhaseZero.onClick = [this]() {
        auto* p = audioProcessor.apvts.getParameter("phase_mode");
        if (p) p->setValueNotifyingHost(p->convertTo0to1(0.0f));
        btnPhaseZero.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff064e3b));
        btnPhaseZero.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff34d399));
        btnPhaseNatural.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
        btnPhaseNatural.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
        btnPhaseLinear.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
        btnPhaseLinear.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
    };

    btnPhaseNatural.onClick = [this]() {
        auto* p = audioProcessor.apvts.getParameter("phase_mode");
        if (p) p->setValueNotifyingHost(p->convertTo0to1(1.0f));
        btnPhaseNatural.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff064e3b));
        btnPhaseNatural.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff34d399));
        btnPhaseZero.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
        btnPhaseZero.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
        btnPhaseLinear.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
        btnPhaseLinear.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
    };

    btnPhaseLinear.onClick = [this]() {
        auto* p = audioProcessor.apvts.getParameter("phase_mode");
        if (p) p->setValueNotifyingHost(p->convertTo0to1(2.0f));
        btnPhaseLinear.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff064e3b));
        btnPhaseLinear.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff34d399));
        btnPhaseZero.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
        btnPhaseZero.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
        btnPhaseNatural.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
        btnPhaseNatural.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
    };

    addAndMakeVisible(togglePianoRoll);
    togglePianoRoll.setToggleState(true, juce::dontSendNotification);
    togglePianoRoll.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffc084fc));
    togglePianoRoll.onClick = [this]() {
        plotComponent.isPianoRollVisible = togglePianoRoll.getToggleState();
        plotComponent.repaint();
    };

    addAndMakeVisible(toggleRtaAnalyzer);
    toggleRtaAnalyzer.setToggleState(true, juce::dontSendNotification);
    toggleRtaAnalyzer.setColour(juce::ToggleButton::textColourId, juce::Colour(0xfffbbf24));
    toggleRtaAnalyzer.onClick = [this]() {
        plotComponent.isRtaAnalyzerVisible = toggleRtaAnalyzer.getToggleState();
        plotComponent.repaint();
    };

    addAndMakeVisible(speedSlider);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setRange(0.1, 1.0, 0.05);
    speedSlider.setValue(0.5);
    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff34d399));
    speedSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff064e3b));

    setupBtn(btnScale3dB, juce::Colour(0xff121722), juce::Colour(0xff64748b));
    setupBtn(btnScale6dB, juce::Colour(0xff121722), juce::Colour(0xff64748b));
    setupBtn(btnScale12dB, juce::Colour(0xff0284c7), juce::Colours::white);
    setupBtn(btnScale30dB, juce::Colour(0xff121722), juce::Colour(0xff64748b));

    auto updateScaleButtons = [this](float sc) {
        plotComponent.currentDbScale = sc;
        btnScale3dB.setColour(juce::TextButton::buttonColourId, sc == 3.0f ? juce::Colour(0xff0284c7) : juce::Colour(0xff121722));
        btnScale3dB.setColour(juce::TextButton::textColourOffId, sc == 3.0f ? juce::Colours::white : juce::Colour(0xff64748b));
        btnScale6dB.setColour(juce::TextButton::buttonColourId, sc == 6.0f ? juce::Colour(0xff0284c7) : juce::Colour(0xff121722));
        btnScale6dB.setColour(juce::TextButton::textColourOffId, sc == 6.0f ? juce::Colours::white : juce::Colour(0xff64748b));
        btnScale12dB.setColour(juce::TextButton::buttonColourId, sc == 12.0f ? juce::Colour(0xff0284c7) : juce::Colour(0xff121722));
        btnScale12dB.setColour(juce::TextButton::textColourOffId, sc == 12.0f ? juce::Colours::white : juce::Colour(0xff64748b));
        btnScale30dB.setColour(juce::TextButton::buttonColourId, sc == 30.0f ? juce::Colour(0xff0284c7) : juce::Colour(0xff121722));
        btnScale30dB.setColour(juce::TextButton::textColourOffId, sc == 30.0f ? juce::Colours::white : juce::Colour(0xff64748b));
        plotComponent.repaint();
    };

    btnScale3dB.onClick = [=]() { updateScaleButtons(3.0f); };
    btnScale6dB.onClick = [=]() { updateScaleButtons(6.0f); };
    btnScale12dB.onClick = [=]() { updateScaleButtons(12.0f); };
    btnScale30dB.onClick = [=]() { updateScaleButtons(30.0f); };

    // Plot Component
    addAndMakeVisible(plotComponent);
    plotComponent.onBandSelected = [this](int bandIdx) {
        updateInspector(bandIdx);
    };
    plotComponent.onBandDragged = [this](int bandIdx) {
        if (bandIdx == plotComponent.getSelectedBand())
        {
            float f = (float)freqSlider.getValue();
            freqValLabel.setText(f < 1000.0f ? juce::String(f, 0) + " Hz" : juce::String(f / 1000.0f, 2) + " kHz", juce::dontSendNotification);
            inspectorNoteBadge.setText(ProQPlotComponent::getNoteNameForFreq(f), juce::dontSendNotification);
            float g = (float)gainSlider.getValue();
            gainValLabel.setText((g > 0 ? "+" : "") + juce::String(g, 1) + "dB", juce::dontSendNotification);
        }
        else
        {
            updateInspector(bandIdx);
        }
    };

    // --- Bottom Floating Inspector Setup ---
    inspectorBandBadge.setFont(juce::Font(15.0f, juce::Font::bold));
    inspectorBandBadge.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(inspectorBandBadge);

    inspectorNoteBadge.setFont(juce::Font(12.0f, juce::Font::bold));
    inspectorNoteBadge.setColour(juce::Label::textColourId, juce::Colour(0xff38bdf8));
    inspectorNoteBadge.setColour(juce::Label::backgroundColourId, juce::Colour(0xff162032));
    inspectorNoteBadge.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(inspectorNoteBadge);

    setupBtn(btnBandPower, juce::Colour(0xff101520), juce::Colour(0xff34d399));
    setupBtn(btnBandSolo, juce::Colour(0xff101520), juce::Colour(0xff94a3b8));
    setupBtn(btnBandPhaseInvert, juce::Colour(0xff101520), juce::Colour(0xff94a3b8));

    btnBandPower.onClick = [this]() {
        int b = plotComponent.getSelectedBand();
        auto* p = audioProcessor.apvts.getParameter(VeviProQAudioProcessor::getParamId(b, "enable"));
        if (p)
        {
            bool cur = p->getValue() > 0.5f;
            p->setValueNotifyingHost(cur ? 0.0f : 1.0f);
            btnBandPower.setColour(juce::TextButton::textColourOffId, !cur ? juce::Colour(0xff34d399) : juce::Colour(0xffef4444));
            plotComponent.repaint();
        }
    };

    // Routing Pills (ST, M, S, L, R)
    static const char* routings[] = { "ST", "M", "S", "L", "R" };
    for (int i = 0; i < 5; ++i)
    {
        routingButtons[(size_t)i].setButtonText(routings[i]);
        setupBtn(routingButtons[(size_t)i], (i == 0 ? juce::Colour(0xff0284c7) : juce::Colour(0xff101520)),
                 (i == 0 ? juce::Colours::white : juce::Colour(0xff94a3b8)));
        routingButtons[(size_t)i].onClick = [this, i]() {
            int b = plotComponent.getSelectedBand();
            auto* p = audioProcessor.apvts.getParameter(VeviProQAudioProcessor::getParamId(b, "routing"));
            if (p != nullptr) p->setValueNotifyingHost(p->convertTo0to1((float)i));
            updateInspector(b);
        };
    }

    // Shape Buttons (Bell, Low Shelf, High Shelf, Low Cut, High Cut, Notch, Band Pass, Tilt)
    static const char* shapes[] = { "Bell", "Low Shelf", "High Shelf", "Low Cut", "High Cut", "Notch", "Band Pass", "Tilt" };
    for (int i = 0; i < 8; ++i)
    {
        shapeButtons[(size_t)i].setButtonText(shapes[i]);
        setupBtn(shapeButtons[(size_t)i], juce::Colour(0xff101520), juce::Colour(0xff94a3b8));
        shapeButtons[(size_t)i].onClick = [this, i]() {
            int b = plotComponent.getSelectedBand();
            auto* p = audioProcessor.apvts.getParameter(VeviProQAudioProcessor::getParamId(b, "type"));
            if (p != nullptr) p->setValueNotifyingHost(p->convertTo0to1((float)i));
            plotComponent.repaint();
            updateInspector(b);
        };
    }

    // Slope Selectors (6dB, 12dB, 18dB, 24dB, 48dB)
    static const char* slopes[] = { "6dB", "12dB", "18dB", "24dB", "48dB" };
    for (int i = 0; i < 5; ++i)
    {
        slopeButtons[(size_t)i].setButtonText(slopes[i]);
        setupBtn(slopeButtons[(size_t)i], (i == 1 ? juce::Colour(0xff6b21a8) : juce::Colour(0xff0d111a)),
                 (i == 1 ? juce::Colour(0xfff3e8ff) : juce::Colour(0xff64748b)));
        slopeButtons[(size_t)i].onClick = [this, i]() {
            int b = plotComponent.getSelectedBand();
            auto* p = audioProcessor.apvts.getParameter(VeviProQAudioProcessor::getParamId(b, "slope"));
            if (p != nullptr) p->setValueNotifyingHost(p->convertTo0to1((float)i));
            updateInspector(b);
        };
    }

    // Setup rotary dials
    auto setupDial = [this](juce::Slider& s, juce::Label& l, juce::Label& val, const juce::String& name) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(s);

        l.setText(name, juce::dontSendNotification);
        l.setFont(juce::Font(10.5f, juce::Font::bold));
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
        addAndMakeVisible(l);

        val.setFont(juce::Font(11.5f));
        val.setJustificationType(juce::Justification::centred);
        val.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(val);
    };

    setupDial(freqSlider, freqLabel, freqValLabel, "FREQ");
    setupDial(gainSlider, gainLabel, gainValLabel, "GAIN");
    setupDial(qSlider, qLabel, qValLabel, "Q");

    freqSlider.onValueChange = [this]() {
        float f = (float)freqSlider.getValue();
        freqValLabel.setText(f < 1000.0f ? juce::String(f, 0) + " Hz" : juce::String(f / 1000.0f, 2) + " kHz", juce::dontSendNotification);
        inspectorNoteBadge.setText(ProQPlotComponent::getNoteNameForFreq(f), juce::dontSendNotification);
        plotComponent.repaint();
    };

    gainSlider.onValueChange = [this]() {
        float g = (float)gainSlider.getValue();
        gainValLabel.setText((g > 0 ? "+" : "") + juce::String(g, 1) + "dB", juce::dontSendNotification);
        plotComponent.repaint();
    };

    qSlider.onValueChange = [this]() {
        qValLabel.setText(juce::String(qSlider.getValue(), 2), juce::dontSendNotification);
        plotComponent.repaint();
    };

    // Dynamic EQ Controls
    addAndMakeVisible(toggleDynamicEq);
    toggleDynamicEq.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffc084fc));

    setupDial(dynRangeSlider, dynRangeLabel, dynRangeValLabel, "RANGE");
    setupDial(dynThreshSlider, dynThreshLabel, dynThreshValLabel, "THRESH");

    dynRangeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffc084fc));
    dynThreshSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xfff43f5e));

    dynRangeSlider.onValueChange = [this]() {
        float r = (float)dynRangeSlider.getValue();
        dynRangeValLabel.setText((r > 0 ? "+" : "") + juce::String(r, 1) + "dB", juce::dontSendNotification);
        plotComponent.repaint();
    };

    dynThreshSlider.onValueChange = [this]() {
        float t = (float)dynThreshSlider.getValue();
        dynThreshValLabel.setText(juce::String((int)t) + "dB", juce::dontSendNotification);
        plotComponent.repaint();
    };

    // Bottom Band Quick-Selector Tabs (Band 1 to Band 7)
    for (int i = 0; i < 7; ++i)
    {
        bandPills[(size_t)i].setButtonText("Band " + juce::String(i + 1));
        auto col = ProQPlotComponent::getBandColour(i);
        bandPills[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff121722));
        bandPills[(size_t)i].setColour(juce::TextButton::textColourOffId, col);
        bandPills[(size_t)i].onClick = [this, i]() {
            plotComponent.setSelectedBand(i);
        };
        addAndMakeVisible(bandPills[(size_t)i]);
    }

    // Master Output Strip (Fully functional and directly accessible)
    setupDial(outputSlider, outputLabel, outputValLabel, "OUTPUT");
    outputSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff38bdf8));
    outputValLabel.setText("0.0dB", juce::dontSendNotification);
    outputSlider.onValueChange = [this]() {
        float v = (float)outputSlider.getValue();
        outputValLabel.setText((v > 0 ? "+" : "") + juce::String(v, 1) + "dB", juce::dontSendNotification);
    };

    outputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "master_gain", outputSlider);

    setupBtn(btnPhaseToggle, juce::Colour(0xff121722), juce::Colour(0xff94a3b8));
    btnPhaseToggle.setButtonText("PHASE");
    btnPhaseToggle.onClick = [this]() {
        // Toggle Master Phase Invert or Cycle Mode
        static bool phaseInv = false;
        phaseInv = !phaseInv;
        btnPhaseToggle.setColour(juce::TextButton::textColourOffId, phaseInv ? juce::Colour(0xfff43f5e) : juce::Colour(0xff94a3b8));
        btnPhaseToggle.setButtonText(phaseInv ? "INV" : "PHASE");
    };

    updateInspector(plotComponent.getSelectedBand());
    refreshPresetsUI();
}

VeviProQAudioProcessorEditor::~VeviProQAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void VeviProQAudioProcessorEditor::updateInspector(int bandIdx)
{
    freqAttach.reset();
    gainAttach.reset();
    qAttach.reset();
    dynEnableAttach.reset();
    dynRangeAttach.reset();
    dynThreshAttach.reset();

    auto bandCol = ProQPlotComponent::getBandColour(bandIdx);

    inspectorBandBadge.setText("Band " + juce::String(bandIdx + 1), juce::dontSendNotification);

    // Update bottom tabs styling
    for (int i = 0; i < 7; ++i)
    {
        auto col = ProQPlotComponent::getBandColour(i);
        if (i == bandIdx)
        {
            bandPills[(size_t)i].setColour(juce::TextButton::buttonColourId, col.withAlpha(0.28f));
            bandPills[(size_t)i].setColour(juce::TextButton::textColourOffId, col);
        }
        else
        {
            bandPills[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
            bandPills[(size_t)i].setColour(juce::TextButton::textColourOffId, col.withAlpha(0.6f));
        }
    }

    // Reattach Knobs with current band's APVTS parameters
    freqAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, VeviProQAudioProcessor::getParamId(bandIdx, "freq"), freqSlider);

    gainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, VeviProQAudioProcessor::getParamId(bandIdx, "gain"), gainSlider);

    qAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, VeviProQAudioProcessor::getParamId(bandIdx, "q"), qSlider);

    dynEnableAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, VeviProQAudioProcessor::getParamId(bandIdx, "dyn_enable"), toggleDynamicEq);

    dynRangeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, VeviProQAudioProcessor::getParamId(bandIdx, "dyn_range"), dynRangeSlider);

    dynThreshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, VeviProQAudioProcessor::getParamId(bandIdx, "dyn_thresh"), dynThreshSlider);

    // Match Knob Fill Color to Band Color
    freqSlider.setColour(juce::Slider::rotarySliderFillColourId, bandCol);
    freqSlider.setColour(juce::Slider::thumbColourId, bandCol);
    gainSlider.setColour(juce::Slider::rotarySliderFillColourId, bandCol);
    gainSlider.setColour(juce::Slider::thumbColourId, bandCol);
    qSlider.setColour(juce::Slider::rotarySliderFillColourId, bandCol);
    qSlider.setColour(juce::Slider::thumbColourId, bandCol);

    // Update values
    float f = (float)freqSlider.getValue();
    freqValLabel.setText(f < 1000.0f ? juce::String(f, 0) + " Hz" : juce::String(f / 1000.0f, 2) + " kHz", juce::dontSendNotification);
    inspectorNoteBadge.setText(ProQPlotComponent::getNoteNameForFreq(f), juce::dontSendNotification);

    float g = (float)gainSlider.getValue();
    gainValLabel.setText((g > 0 ? "+" : "") + juce::String(g, 1) + "dB", juce::dontSendNotification);
    qValLabel.setText(juce::String(qSlider.getValue(), 2), juce::dontSendNotification);

    float r = (float)dynRangeSlider.getValue();
    dynRangeValLabel.setText((r > 0 ? "+" : "") + juce::String(r, 1) + "dB", juce::dontSendNotification);
    dynThreshValLabel.setText(juce::String((int)dynThreshSlider.getValue()) + "dB", juce::dontSendNotification);

    // Update active shape button
    auto s = audioProcessor.getBandSettings(bandIdx);
    for (int i = 0; i < 8; ++i)
    {
        if (i == (int)s.type)
        {
            shapeButtons[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff0369a1));
            shapeButtons[(size_t)i].setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        }
        else
        {
            shapeButtons[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
            shapeButtons[(size_t)i].setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
        }
    }

    // Update active slope button
    for (int i = 0; i < 5; ++i)
    {
        if (i == (s.slope == 6 ? 0 : s.slope == 12 ? 1 : s.slope == 18 ? 2 : s.slope == 24 ? 3 : 4))
        {
            slopeButtons[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff6b21a8));
            slopeButtons[(size_t)i].setColour(juce::TextButton::textColourOffId, juce::Colour(0xfff3e8ff));
        }
        else
        {
            slopeButtons[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff0d111a));
            slopeButtons[(size_t)i].setColour(juce::TextButton::textColourOffId, juce::Colour(0xff64748b));
        }
    }

    // Update active routing button
    for (int i = 0; i < 5; ++i)
    {
        if (i == s.stereoRouting)
        {
            routingButtons[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff0284c7));
            routingButtons[(size_t)i].setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        }
        else
        {
            routingButtons[(size_t)i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff101520));
            routingButtons[(size_t)i].setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
        }
    }

    btnBandPower.setColour(juce::TextButton::textColourOffId, s.enabled ? juce::Colour(0xff34d399) : juce::Colour(0xffef4444));
}

void VeviProQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff05070a));

    // Top Header Background
    g.setColour(juce::Colour(0xff090c13));
    g.fillRect(0, 0, getWidth(), 90);
    g.setColour(juce::Colour(0xff182030));
    g.drawHorizontalLine(90, 0.0f, (float)getWidth());

    // Logo Icon & Title
    g.setColour(juce::Colour(0xff38bdf8));
    juce::Path logo;
    logo.startNewSubPath(20, 28);
    logo.cubicTo(28, 16, 32, 38, 42, 28);
    g.strokePath(logo, juce::PathStrokeType(2.5f));

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("VEVI Pro-Q", 52, 17, 120, 22, juce::Justification::left);

    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff64748b));
    g.drawText("7-BAND DYNAMIC EQ", 52, 37, 140, 14, juce::Justification::left);

    // FL STUDIO VST3 orange badge
    g.setColour(juce::Colour(0xffea580c).withAlpha(0.25f));
    g.fillRoundedRectangle(192, 37, 95, 15, 3.0f);
    g.setColour(juce::Colour(0xfffb923c));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("FL STUDIO VST3", 192, 37, 95, 15, juce::Justification::centred);

    // v1.5 badge
    g.setColour(juce::Colour(0xff3b82f6).withAlpha(0.25f));
    g.fillRoundedRectangle(292, 37, 40, 15, 3.0f);
    g.setColour(juce::Colour(0xff60a5fa));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("v1.5", 292, 37, 40, 15, juce::Justification::centred);

    // Subheader labels & Analyzer Legend (Pre-EQ Raw vs Post-EQ)
    g.setColour(juce::Colour(0xff64748b));
    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.drawText("PHASE:", 20, 62, 45, 22, juce::Justification::left);

    // Visual legend for Pre / Post Spectrum
    g.setColour(juce::Colour(0xff38bdf8));
    g.fillEllipse((float)650, 69.0f, 6.0f, 6.0f);
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xff94a3b8));
    g.drawText("IN (Raw)", 660, 62, 50, 22, juce::Justification::left);

    g.setColour(juce::Colour(0xfffbbf24));
    g.fillEllipse((float)718, 69.0f, 6.0f, 6.0f);
    g.setColour(juce::Colour(0xff94a3b8));
    g.drawText("OUT (EQ)", 728, 62, 55, 22, juce::Justification::left);

    g.drawText("Speed:", getWidth() - 440, 62, 45, 22, juce::Justification::left);
    g.drawText("SCALE", getWidth() - 300, 62, 45, 22, juce::Justification::left);

    // Master Output Strip Divider (Right side)
    int outX = getWidth() - 95;
    g.setColour(juce::Colour(0xff151b27));
    g.drawVerticalLine(outX - 12, 90.0f, (float)getHeight());

    // Output Level Meters (Dual LED bar)
    g.setColour(juce::Colour(0xff090d14));
    g.fillRoundedRectangle((float)outX + 16.0f, 235.0f, 20.0f, 180.0f, 4.0f);
    g.setColour(juce::Colour(0xff182232));
    g.drawRoundedRectangle((float)outX + 16.0f, 235.0f, 20.0f, 180.0f, 4.0f, 1.0f);

    // Active Green to Amber LED Level in meter
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff34d399), (float)outX + 18.0f, 410.0f,
                                           juce::Colour(0xfff59e0b), (float)outX + 18.0f, 270.0f, false));
    g.fillRoundedRectangle((float)outX + 18.0f, 285.0f, 16.0f, 128.0f, 2.0f);

    // Floating Inspector Dock Background
    int inspW = getWidth() - 210;
    int inspH = 175;
    int inspX = 50;
    int inspY = getHeight() - 235;

    g.setColour(juce::Colour(0xf2090d15));
    g.fillRoundedRectangle((float)inspX, (float)inspY, (float)inspW, (float)inspH, 12.0f);
    g.setColour(juce::Colour(0xff1e293b));
    g.drawRoundedRectangle((float)inspX, (float)inspY, (float)inspW, (float)inspH, 12.0f, 1.2f);

    // Inspector section labels
    g.setColour(juce::Colour(0xff64748b));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("SHAPE", inspX + 180, inspY + 14, 60, 14, juce::Justification::left);
    g.drawText("SLOPE", inspX + 180, inspY + 135, 60, 14, juce::Justification::left);

    // Dividers inside Inspector
    g.setColour(juce::Colour(0xff161f2e));
    g.drawVerticalLine(inspX + 165, (float)inspY + 12, (float)inspY + inspH - 12);
    g.drawVerticalLine(inspX + 490, (float)inspY + 12, (float)inspY + inspH - 12);
    g.drawVerticalLine(inspX + 750, (float)inspY + 12, (float)inspY + inspH - 12);
}

void VeviProQAudioProcessorEditor::refreshPresetsUI()
{
    int cur = audioProcessor.getCurrentProgram();
    auto name = audioProcessor.getProgramName(cur);
    btnPresetName.setButtonText(name);
    updateInspector(plotComponent.getSelectedBand());
    plotComponent.repaint();
}

void VeviProQAudioProcessorEditor::resized()
{
    // --- Header Row 1 (y: 14) ---
    // Clean, perfectly spaced without overlapping logo or other controls (Logo + Badges span up to x ~ 335)
    btnAiMaster.setBounds(350, 14, 88, 26);
    btnPower.setBounds(444, 14, 62, 26);
    btnFullscreen.setBounds(512, 14, 48, 26);

    // Center Preset Bar
    int presetX = 575;
    btnPrevPreset.setBounds(presetX, 14, 28, 26);
    btnPresetName.setBounds(presetX + 32, 14, 245, 26);
    btnNextPreset.setBounds(presetX + 281, 14, 28, 26);
    btnABCompare.setBounds(presetX + 315, 14, 65, 26);

    // Right Header Bar
    int rightX = getWidth() - 390;
    btnTestAudio.setBounds(rightX, 14, 105, 26);
    sampleBeatCombo.setBounds(rightX + 112, 14, 255, 26);

    // --- Subheader Row 2 (y: 60) ---
    btnPhaseZero.setBounds(70, 60, 95, 24);
    btnPhaseNatural.setBounds(172, 60, 105, 24);
    btnPhaseLinear.setBounds(284, 60, 95, 24);

    togglePianoRoll.setBounds(395, 60, 110, 24);
    toggleRtaAnalyzer.setBounds(515, 60, 125, 24);

    speedSlider.setBounds(getWidth() - 390, 62, 80, 20);
    btnScale3dB.setBounds(getWidth() - 250, 60, 52, 22);
    btnScale6dB.setBounds(getWidth() - 192, 60, 52, 22);
    btnScale12dB.setBounds(getWidth() - 134, 60, 56, 22);
    btnScale30dB.setBounds(getWidth() - 72, 60, 56, 22);

    // Main Plot Screen (Clears space on right for Master Output)
    plotComponent.setBounds(12, 92, getWidth() - 120, getHeight() - 105);

    // --- Floating Inspector Dock ---
    int inspW = getWidth() - 210;
    int inspH = 175;
    int inspX = 50;
    int inspY = getHeight() - 235;

    // Col 1: Band badge, Power, Solo, Invert, Routing
    inspectorBandBadge.setBounds(inspX + 48, inspY + 14, 75, 24);
    inspectorNoteBadge.setBounds(inspX + 125, inspY + 16, 34, 20);
    btnBandPower.setBounds(inspX + 16, inspY + 50, 38, 28);
    btnBandSolo.setBounds(inspX + 58, inspY + 50, 58, 28);
    btnBandPhaseInvert.setBounds(inspX + 120, inspY + 50, 38, 28);

    int rX = inspX + 16;
    for (int i = 0; i < 5; ++i)
    {
        routingButtons[(size_t)i].setBounds(rX + i * 28, inspY + 90, 26, 24);
    }

    // Col 2: Shape grid (2 rows x 4 cols) & Slope buttons
    int shapeX = inspX + 180;
    int shapeY = inspY + 38;
    int sW = 72;
    int sH = 26;
    for (int i = 0; i < 4; ++i)
        shapeButtons[(size_t)i].setBounds(shapeX + i * (sW + 4), shapeY, sW, sH);
    for (int i = 4; i < 8; ++i)
        shapeButtons[(size_t)i].setBounds(shapeX + (i - 4) * (sW + 4), shapeY + sH + 6, sW, sH);

    int slopeX = inspX + 245;
    for (int i = 0; i < 5; ++i)
    {
        slopeButtons[(size_t)i].setBounds(slopeX + i * 46, inspY + 132, 42, 22);
    }

    // Col 3: Knobs (FREQ, GAIN, Q)
    int dialSize = 68;
    int kX = inspX + 510;
    int gap = 76;

    freqSlider.setBounds(kX, inspY + 26, dialSize, dialSize);
    freqLabel.setBounds(kX, inspY + 98, dialSize, 14);
    freqValLabel.setBounds(kX - 6, inspY + 114, dialSize + 12, 16);

    gainSlider.setBounds(kX + gap, inspY + 26, dialSize, dialSize);
    gainLabel.setBounds(kX + gap, inspY + 98, dialSize, 14);
    gainValLabel.setBounds(kX + gap - 6, inspY + 114, dialSize + 12, 16);

    qSlider.setBounds(kX + gap * 2, inspY + 26, dialSize, dialSize);
    qLabel.setBounds(kX + gap * 2, inspY + 98, dialSize, 14);
    qValLabel.setBounds(kX + gap * 2 - 6, inspY + 114, dialSize + 12, 16);

    // Col 4: Dynamic EQ (RANGE, THRESH)
    int dynX = inspX + 768;
    toggleDynamicEq.setBounds(dynX, inspY + 14, 125, 24);

    int dynDialSize = 58;
    dynRangeSlider.setBounds(dynX, inspY + 44, dynDialSize, dynDialSize);
    dynRangeLabel.setBounds(dynX, inspY + 104, dynDialSize, 14);
    dynRangeValLabel.setBounds(dynX - 6, inspY + 120, dynDialSize + 12, 16);

    dynThreshSlider.setBounds(dynX + 66, inspY + 44, dynDialSize, dynDialSize);
    dynThreshLabel.setBounds(dynX + 66, inspY + 104, dynDialSize, 14);
    dynThreshValLabel.setBounds(dynX + 60, inspY + 120, dynDialSize + 12, 16);

    // --- Bottom Band Tabs (Band 1 to Band 7) ---
    int tabW = 82;
    int tabH = 26;
    int tabStartX = (getWidth() - 100 - (7 * (tabW + 8))) / 2;
    int tabY = getHeight() - 40;

    for (int i = 0; i < 7; ++i)
    {
        bandPills[(size_t)i].setBounds(tabStartX + i * (tabW + 8), tabY, tabW, tabH);
    }

    // --- Right Output Strip ---
    int outX = getWidth() - 88;
    outputSlider.setBounds(outX, 105, 62, 62);
    outputLabel.setBounds(outX, 172, 62, 14);
    outputValLabel.setBounds(outX, 188, 62, 16);
    btnPhaseToggle.setBounds(outX + 10, 208, 42, 22);
}
