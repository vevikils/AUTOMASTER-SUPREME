#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <array>
#include <cmath>

namespace VeviDSP
{

enum FilterType
{
    Bell = 0,
    LowCut,
    LowShelf,
    HighShelf,
    HighCut,
    Notch
};

struct BandSettings
{
    bool enabled = true;
    FilterType type = Bell;
    float frequency = 1000.0f;
    float gainDB = 0.0f;
    float q = 0.707f;

    // Dynamic EQ Settings (FabFilter style)
    bool dynamicEnabled = false;
    float dynamicRangeDB = 0.0f;  // Can be positive (expansion) or negative (compression)
    float thresholdDB = -18.0f;   // Dynamic threshold
    float attackMs = 15.0f;
    float releaseMs = 120.0f;
};

// ==============================================================================
// Envelope Follower for Sidechain / Band Detector
// ==============================================================================
class EnvelopeFollower
{
public:
    EnvelopeFollower() = default;

    void prepare(double sr)
    {
        sampleRate = sr > 0 ? sr : 44100.0;
        envelope = 0.0f;
        updateCoefficients(attackMs, releaseMs);
    }

    void updateCoefficients(float attack, float release)
    {
        attackMs = attack;
        releaseMs = release;
        attackCoeff = std::exp(-1.0f / (float(sampleRate) * (attackMs * 0.001f)));
        releaseCoeff = std::exp(-1.0f / (float(sampleRate) * (releaseMs * 0.001f)));
    }

    float processSample(float input)
    {
        float absVal = std::abs(input);
        if (absVal > envelope)
            envelope = attackCoeff * envelope + (1.0f - attackCoeff) * absVal;
        else
            envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * absVal;

        return envelope;
    }

    void reset()
    {
        envelope = 0.0f;
    }

    float getCurrentLevelDB() const
    {
        return juce::Decibels::gainToDecibels(envelope + 1e-6f);
    }

private:
    double sampleRate = 44100.0;
    float attackMs = 15.0f;
    float releaseMs = 120.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float envelope = 0.0f;
};

// ==============================================================================
// Dynamic Band Filter
// ==============================================================================
class DynamicBandFilter
{
public:
    DynamicBandFilter() = default;

    void prepare(double sr, int samplesPerBlock)
    {
        sampleRate = sr > 0 ? sr : 44100.0;

        envFollowerLeft.prepare(sampleRate);
        envFollowerRight.prepare(sampleRate);

        for (int ch = 0; ch < 2; ++ch)
        {
            iir[ch].reset();
            detectorFilter[ch].reset();
        }

        currentDynamicGainDB = 0.0f;
        computedGainDB = 0.0f;
    }

    void reset()
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            iir[ch].reset();
            detectorFilter[ch].reset();
        }

        envFollowerLeft.reset();
        envFollowerRight.reset();
        currentDynamicGainDB = 0.0f;
    }

    // Process a block of stereo samples
    void process(juce::AudioBuffer<float>& buffer, const BandSettings& settings)
    {
        if (!settings.enabled)
            return;

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        // 1. Calculate dynamic gain mod if dynamic is active
        float dynamicGainMod = 0.0f;

        if (settings.dynamicEnabled && std::abs(settings.dynamicRangeDB) > 0.01f)
        {
            // Update detector bandpass filter around the band frequency
            auto detectorCoeffs = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, settings.frequency, settings.q);
            detectorFilter[0].coefficients = detectorCoeffs;
            detectorFilter[1].coefficients = detectorCoeffs;

            // Process detector signal
            float maxEnv = 0.0f;
            const float* const* readPtrs = buffer.getArrayOfReadPointers();

            for (int i = 0; i < numSamples; ++i)
            {
                float leftSample = readPtrs[0][i];
                float rightSample = (numChannels > 1) ? readPtrs[1][i] : leftSample;

                // Simple filter detector
                float detLeft = detectorFilter[0].processSample(leftSample);
                float detRight = (numChannels > 1) ? detectorFilter[1].processSample(rightSample) : detLeft;

                float envL = envFollowerLeft.processSample(detLeft);
                float envR = envFollowerRight.processSample(detRight);

                float peak = std::max(envL, envR);
                if (peak > maxEnv)
                    maxEnv = peak;
            }

            float envDB = juce::Decibels::gainToDecibels(maxEnv + 1e-6f);
            float overshoot = envDB - settings.thresholdDB;

            if (overshoot > 0.0f)
            {
                // Soft-knee curve factor
                float ratio = std::clamp(overshoot / 24.0f, 0.0f, 1.0f);
                dynamicGainMod = settings.dynamicRangeDB * ratio;
            }
        }

        // Smooth dynamic gain
        currentDynamicGainDB = currentDynamicGainDB * 0.85f + dynamicGainMod * 0.15f;
        computedGainDB = settings.gainDB + currentDynamicGainDB;

        // 2. Compute filter coefficients
        updateCoefficients(settings, computedGainDB);

        // 3. Process channels through main filter
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                channelData[i] = iir[ch].processSample(channelData[i]);
            }
        }
    }

    void updateCoefficients(const BandSettings& settings, float activeGainDB)
    {
        juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> newCoeffs;

        float freq = std::clamp(settings.frequency, 20.0f, float(sampleRate * 0.495));
        float q = std::clamp(settings.q, 0.1f, 30.0f);
        float gainLinear = juce::Decibels::decibelsToGain(activeGainDB);

        switch (settings.type)
        {
            case FilterType::LowCut:
                *coeffs = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, freq, q);
                break;
            case FilterType::LowShelf:
                *coeffs = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, freq, q, gainLinear);
                break;
            case FilterType::HighShelf:
                *coeffs = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, freq, q, gainLinear);
                break;
            case FilterType::HighCut:
                *coeffs = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq, q);
                break;
            case FilterType::Notch:
                *coeffs = *juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, freq, q);
                break;
            case FilterType::Bell:
            default:
                *coeffs = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, freq, q, gainLinear);
                break;
        }

        for (int ch = 0; ch < 2; ++ch)
        {
            iir[ch].coefficients = coeffs;
        }
    }

    // Evaluate magnitude response for GUI visualization
    double getMagnitudeForFrequency(double freq, const BandSettings& settings) const
    {
        if (!settings.enabled)
            return 1.0;

        float gainLinear = juce::Decibels::decibelsToGain(settings.gainDB + currentDynamicGainDB);
        float f = std::clamp(settings.frequency, 20.0f, float(sampleRate * 0.495));
        float q = std::clamp(settings.q, 0.1f, 30.0f);

        juce::dsp::IIR::Coefficients<float>::Ptr tempCoeffs;

        switch (settings.type)
        {
            case FilterType::LowCut:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, f, q);
                break;
            case FilterType::LowShelf:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, f, q, gainLinear);
                break;
            case FilterType::HighShelf:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, f, q, gainLinear);
                break;
            case FilterType::HighCut:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, f, q);
                break;
            case FilterType::Notch:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, f, q);
                break;
            case FilterType::Bell:
            default:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, f, q, gainLinear);
                break;
        }

        if (tempCoeffs != nullptr)
            return tempCoeffs->getMagnitudeForFrequency(freq, sampleRate);

        return 1.0;
    }

    static double computeMagnitude(double freq, double sr, const BandSettings& settings, float customGainDB)
    {
        if (!settings.enabled)
            return 1.0;

        float gainLinear = juce::Decibels::decibelsToGain(customGainDB);
        float f = std::clamp(settings.frequency, 20.0f, float(sr * 0.495));
        float q = std::clamp(settings.q, 0.1f, 30.0f);

        juce::dsp::IIR::Coefficients<float>::Ptr tempCoeffs;

        switch (settings.type)
        {
            case FilterType::LowCut:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, f, q);
                break;
            case FilterType::LowShelf:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sr, f, q, gainLinear);
                break;
            case FilterType::HighShelf:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sr, f, q, gainLinear);
                break;
            case FilterType::HighCut:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, f, q);
                break;
            case FilterType::Notch:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makeNotch(sr, f, q);
                break;
            case FilterType::Bell:
            default:
                tempCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, f, q, gainLinear);
                break;
        }

        if (tempCoeffs != nullptr)
            return tempCoeffs->getMagnitudeForFrequency(freq, sr);

        return 1.0;
    }

    float getCurrentDynamicGainDB() const { return currentDynamicGainDB; }

private:
    double sampleRate = 44100.0;
    juce::dsp::IIR::Coefficients<float>::Ptr coeffs = new juce::dsp::IIR::Coefficients<float>();
    juce::dsp::IIR::Filter<float> iir[2];
    juce::dsp::IIR::Filter<float> detectorFilter[2];
    EnvelopeFollower envFollowerLeft;
    EnvelopeFollower envFollowerRight;

    float currentDynamicGainDB = 0.0f;
    float computedGainDB = 0.0f;
};

// ==============================================================================
// 8-Band Dynamic EQ DSP Engine
// ==============================================================================
class DynamicEQEngine
{
public:
    static constexpr int NUM_BANDS = 8;

    DynamicEQEngine()
    {
        // Initial defaults for 8 bands
        defaultBands[0] = { true, FilterType::LowCut,    35.0f,     0.0f, 0.707f, false,  0.0f, -18.0f, 15.0f, 100.0f };
        defaultBands[1] = { true, FilterType::LowShelf,  100.0f,    0.0f, 0.707f, false,  0.0f, -18.0f, 15.0f, 100.0f };
        defaultBands[2] = { true, FilterType::Bell,      250.0f,    0.0f, 1.0f,   false,  0.0f, -18.0f, 15.0f, 100.0f };
        defaultBands[3] = { true, FilterType::Bell,      800.0f,    0.0f, 1.2f,   false,  0.0f, -18.0f, 15.0f, 100.0f };
        defaultBands[4] = { true, FilterType::Bell,      2500.0f,   0.0f, 1.2f,   true,  -3.0f, -18.0f, 10.0f,  80.0f }; // Dynamic De-Harsh
        defaultBands[5] = { true, FilterType::Bell,      5000.0f,   0.0f, 1.5f,   false,  0.0f, -18.0f, 15.0f, 100.0f };
        defaultBands[6] = { true, FilterType::HighShelf, 10000.0f,  0.0f, 0.707f, false,  0.0f, -18.0f, 15.0f, 100.0f };
        defaultBands[7] = { true, FilterType::HighCut,   18500.0f,  0.0f, 0.707f, false,  0.0f, -18.0f, 15.0f, 100.0f };
    }

    void prepare(double sr, int samplesPerBlock)
    {
        sampleRate = sr;
        for (int i = 0; i < NUM_BANDS; ++i)
        {
            bands[i].prepare(sr, samplesPerBlock);
        }
    }

    void reset()
    {
        for (int i = 0; i < NUM_BANDS; ++i)
            bands[i].reset();
    }

    void process(juce::AudioBuffer<float>& buffer, const std::array<BandSettings, NUM_BANDS>& settings, float outputGainDB)
    {
        for (int i = 0; i < NUM_BANDS; ++i)
        {
            bands[i].process(buffer, settings[i]);
        }

        if (std::abs(outputGainDB) > 0.01f)
        {
            buffer.applyGain(juce::Decibels::decibelsToGain(outputGainDB));
        }
    }

    double getMagnitudeForFrequency(double freq, const std::array<BandSettings, NUM_BANDS>& settings) const
    {
        double mag = 1.0;
        for (int i = 0; i < NUM_BANDS; ++i)
        {
            mag *= bands[i].getMagnitudeForFrequency(freq, settings[i]);
        }
        return mag;
    }

    double getBandMagnitudeForFrequency(int bandIdx, double freq, const BandSettings& setting) const
    {
        if (bandIdx >= 0 && bandIdx < NUM_BANDS)
            return bands[bandIdx].getMagnitudeForFrequency(freq, setting);
        return 1.0;
    }

    double getBandMagnitudeWithGain(int bandIdx, double freq, const BandSettings& setting, float gainDB) const
    {
        if (bandIdx >= 0 && bandIdx < NUM_BANDS)
            return DynamicBandFilter::computeMagnitude(freq, sampleRate, setting, gainDB);
        return 1.0;
    }

    float getBandDynamicGain(int bandIdx) const
    {
        if (bandIdx >= 0 && bandIdx < NUM_BANDS)
            return bands[bandIdx].getCurrentDynamicGainDB();
        return 0.0f;
    }

    const std::array<BandSettings, NUM_BANDS>& getDefaultBands() const { return defaultBands; }

private:
    double sampleRate = 44100.0;
    std::array<DynamicBandFilter, NUM_BANDS> bands;
    std::array<BandSettings, NUM_BANDS> defaultBands;
};

} // namespace VeviDSP
