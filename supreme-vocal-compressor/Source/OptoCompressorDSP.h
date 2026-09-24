#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <array>
#include <algorithm>

namespace SupremeOptoConstants
{
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float DENORMAL_PROTECT = 1e-18f;
}

class OptoCell
{
public:
    OptoCell() = default;

    void prepare(double sampleRate)
    {
        fs = static_cast<float>(sampleRate);
        reset();
    }

    void reset()
    {
        envelopeFast = 0.0f;
        envelopeSlow = 0.0f;
        gainReductionDb = 0.0f;
    }

    // T4B Opto Cell: non-linear dual time-constant RC release + program-dependent attack
    // Peak reduction: 0 to 100
    // Mode: 0 = Compress (~3:1 soft knee), 1 = Limit (~10:1 to infinity)
    // Emphasis: High-frequency sidechain trim (flat or vocal clarity boost)
    float processSample(float inputLevelDb, float peakReduction, bool isLimiter, float hfEmphasis)
    {
        // 1. Threshold calculation based on Peak Reduction
        // Peak Reduction 0 -> Threshold ~ 0 dB (no compression)
        // Peak Reduction 100 -> Threshold ~ -50 dB
        float thresholdDb = - (peakReduction * 0.52f);

        // HF Emphasis (HF sidechain tilt)
        float weightedInputDb = inputLevelDb + (hfEmphasis * 0.06f);

        // 2. Overshoot above threshold with smooth soft knee
        float overDb = weightedInputDb - thresholdDb;

        float targetGrDb = 0.0f;
        if (overDb > 0.0f)
        {
            if (isLimiter)
            {
                // Limiter mode: High ratio ~ 12:1 to 20:1
                float ratio = 12.0f + (peakReduction * 0.1f);
                targetGrDb = overDb * (1.0f - (1.0f / ratio));
            }
            else
            {
                // Compress mode: Soft progressive optical ratio ~ 2.5:1 to 4:1
                float softKnee = 6.0f; // 6 dB soft knee
                if (overDb < softKnee)
                {
                    float factor = (overDb / softKnee);
                    targetGrDb = (overDb * 0.65f) * (factor * factor);
                }
                else
                {
                    targetGrDb = 0.65f * (overDb - (softKnee * 0.35f));
                }
            }
        }

        // Clamp maximum optical compression to 40 dB
        targetGrDb = std::min(targetGrDb, 40.0f);

        // 3. T4B Dual-Stage Ballistics:
        // Attack: ~ 10 ms (program dependent, faster for loud transients)
        float attackMs = 10.0f / (1.0f + 0.03f * targetGrDb);
        float alphaAttack = 1.0f - std::exp(-1.0f / (fs * (attackMs * 0.001f)));

        // Release:
        // Stage 1 (Fast stage): First ~ 50% release occurs in 60 ms
        // Stage 2 (Slow stage): Memory effect / tail takes 0.5s to 3.0s depending on history
        float releaseFastMs = 60.0f;
        float releaseSlowMs = 800.0f + (envelopeSlow * 60.0f); // Memory accumulation

        float alphaFast = 1.0f - std::exp(-1.0f / (fs * (releaseFastMs * 0.001f)));
        float alphaSlow = 1.0f - std::exp(-1.0f / (fs * (releaseSlowMs * 0.001f)));

        if (targetGrDb > envelopeFast)
        {
            // Attacking
            envelopeFast += alphaAttack * (targetGrDb - envelopeFast);
            envelopeSlow += (alphaAttack * 0.35f) * (targetGrDb - envelopeSlow);
        }
        else
        {
            // Releasing: dual blend
            envelopeFast += alphaFast * (targetGrDb - envelopeFast);
            envelopeSlow += alphaSlow * (targetGrDb - envelopeSlow);
        }

        // Anti-denormal protection
        if (std::abs(envelopeFast) < SupremeOptoConstants::DENORMAL_PROTECT) envelopeFast = 0.0f;
        if (std::abs(envelopeSlow) < SupremeOptoConstants::DENORMAL_PROTECT) envelopeSlow = 0.0f;

        // Combined optical response (60% fast photocell, 40% slow chemical phosphor recovery)
        gainReductionDb = 0.58f * envelopeFast + 0.42f * envelopeSlow;
        return gainReductionDb;
    }

    float getGainReductionDb() const { return gainReductionDb; }

private:
    float fs = 44100.0f;
    float envelopeFast = 0.0f;
    float envelopeSlow = 0.0f;
    float gainReductionDb = 0.0f;
};

class SupremeHarmonicSheen
{
public:
    SupremeHarmonicSheen() = default;

    void prepare(double sampleRate)
    {
        fs = static_cast<float>(sampleRate);
        // High shelf / Air exciter band at 10 kHz
        highShelfFilterL.reset();
        highShelfFilterR.reset();
        updateFilters();
    }

    void reset()
    {
        dcBlockerL = 0.0f;
        dcBlockerR = 0.0f;
    }

    // Sheen Amount: 0 to 100%
    // Sheen Freq: 8 kHz to 16 kHz
    // Tube Drive: 0 to 100%
    void process(float& left, float& right, float sheenAmount, float sheenFreq, float tubeDrive)
    {
        // 1. Vintage 12AX7 / 6V6 Tube Triode Warmth
        // Asymmetric soft-clipping producing musical 2nd and 3rd harmonics
        if (tubeDrive > 0.01f)
        {
            float driveGain = 1.0f + (tubeDrive * 0.025f);
            left = saturateTube(left * driveGain);
            right = saturateTube(right * driveGain);
        }

        // 2. Vocal Air Sheen Exciter (Dynamic harmonic bloom in high frequencies)
        if (sheenAmount > 0.5f)
        {
            float sheenFactor = (sheenAmount * 0.01f);
            
            // Extract high-band
            float highL = left;
            float highR = right;
            
            // Simple 1st-order high pass extractor tuned to sheenFreq
            float rc = 1.0f / (2.0f * SupremeOptoConstants::PI * sheenFreq);
            float dt = 1.0f / fs;
            float alpha = rc / (rc + dt);

            highFilterStateL = alpha * (highFilterStateL + left - prevInputL);
            prevInputL = left;
            highL = left - highFilterStateL;

            highFilterStateR = alpha * (highFilterStateR + right - prevInputR);
            prevInputR = right;
            highR = right - highFilterStateR;

            // Generate silky even harmonics on high frequencies (vacuum tube sheen)
            float harmonicL = 0.5f * (std::abs(highL) - 0.2f * (highL * highL));
            float harmonicR = 0.5f * (std::abs(highR) - 0.2f * (highR * highR));

            // Inject sheen back
            left += harmonicL * (sheenFactor * 0.45f);
            right += harmonicR * (sheenFactor * 0.45f);
        }

        // 3. Simple DC Blocker to keep signal centered
        dcBlockerL = left - dcBlockerL * 0.9995f;
        left -= dcBlockerL * 0.0005f;
        dcBlockerR = right - dcBlockerR * 0.9995f;
        right -= dcBlockerR * 0.0005f;
    }

private:
    float fs = 44100.0f;
    float highFilterStateL = 0.0f, highFilterStateR = 0.0f;
    float prevInputL = 0.0f, prevInputR = 0.0f;
    float dcBlockerL = 0.0f, dcBlockerR = 0.0f;
    juce::dsp::IIR::Filter<float> highShelfFilterL, highShelfFilterR;

    void updateFilters() {}

    inline float saturateTube(float x)
    {
        // 12AX7 Vacuum Tube curve: Asymmetric tanh with slight 2nd harmonic bias (+0.12)
        float biased = x + 0.10f * (x * x);
        if (biased > 1.25f) return 1.0f;
        if (biased < -1.25f) return -1.0f;
        return std::tanh(biased);
    }
};

class SupremeVocalBusDSP
{
public:
    SupremeVocalBusDSP() = default;

    void prepare(double sampleRate, int samplesPerBlock)
    {
        currentSampleRate = sampleRate;
        optoLeft.prepare(sampleRate);
        optoRight.prepare(sampleRate);
        harmonics.prepare(sampleRate);
        reset();
    }

    void reset()
    {
        optoLeft.reset();
        optoRight.reset();
        harmonics.reset();
        currentGrMeterDb.store(0.0f);
        currentInMeterDb.store(-60.0f);
        currentOutMeterDb.store(-60.0f);
    }

    // Parameters:
    // peakReduction: 0 to 100
    // gain: 0 to 100 (Make-up gain: 0 to +40 dB)
    // isLimiter: false = Compress, true = Limit
    // sheenAmount: 0 to 100%
    // sheenFreq: 8000 to 16000 Hz
    // tubeWarmth: 0 to 100%
    // hfEmphasis: -100 to +100 (Sidechain emphasis)
    // dryWet: 0 to 100%
    void processBlock(juce::AudioBuffer<float>& buffer,
                      float peakReduction,
                      float gainMakeup,
                      bool isLimiter,
                      float sheenAmount,
                      float sheenFreq,
                      float tubeWarmth,
                      float hfEmphasis,
                      float dryWet)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        if (numChannels == 0 || numSamples == 0) return;

        // Makeup gain linear factor (0-100 maps to 0dB - +38dB)
        float makeupDb = (gainMakeup * 0.38f);
        float makeupGain = std::pow(10.0f, makeupDb / 20.0f);
        float mixWet = dryWet * 0.01f;
        float mixDry = 1.0f - mixWet;

        float maxPeakIn = 0.0f;
        float maxPeakOut = 0.0f;
        float maxGrBlock = 0.0f;

        float* channelDataL = buffer.getWritePointer(0);
        float* channelDataR = numChannels > 1 ? buffer.getWritePointer(1) : channelDataL;

        for (int i = 0; i < numSamples; ++i)
        {
            float inL = channelDataL[i];
            float inR = channelDataR[i];

            maxPeakIn = std::max(maxPeakIn, std::max(std::abs(inL), std::abs(inR)));

            // 1. Calculate sidechain detector input (stereo-linked peak)
            float detectorLevel = std::max(std::abs(inL), std::abs(inR));
            float detectorDb = (detectorLevel > 1e-5f) ? (20.0f * std::log10(detectorLevel)) : -100.0f;

            // 2. Optical T4B cell compression
            float grDb = optoLeft.processSample(detectorDb, peakReduction, isLimiter, hfEmphasis);
            maxGrBlock = std::max(maxGrBlock, grDb);

            float linearGr = std::pow(10.0f, -grDb / 20.0f);

            // Apply compression & makeup gain
            float compressedL = inL * linearGr * makeupGain;
            float compressedR = inR * linearGr * makeupGain;

            // 3. Process Tube Saturation & Sheen Brilliance
            harmonics.process(compressedL, compressedR, sheenAmount, sheenFreq, tubeWarmth);

            // 4. Dry/Wet Mix
            float outL = inL * mixDry + compressedL * mixWet;
            float outR = inR * mixDry + compressedR * mixWet;

            channelDataL[i] = outL;
            if (numChannels > 1)
                channelDataR[i] = outR;

            maxPeakOut = std::max(maxPeakOut, std::max(std::abs(outL), std::abs(outR)));
        }

        // Store visual meters for UI
        float inDb = (maxPeakIn > 1e-4f) ? (20.0f * std::log10(maxPeakIn)) : -60.0f;
        float outDb = (maxPeakOut > 1e-4f) ? (20.0f * std::log10(maxPeakOut)) : -60.0f;

        // Smooth meter updates
        currentGrMeterDb.store(maxGrBlock);
        currentInMeterDb.store(inDb);
        currentOutMeterDb.store(outDb);
    }

    float getGainReductionDb() const { return currentGrMeterDb.load(); }
    float getInputMeterDb() const { return currentInMeterDb.load(); }
    float getOutputMeterDb() const { return currentOutMeterDb.load(); }

private:
    double currentSampleRate = 44100.0;
    OptoCell optoLeft;
    OptoCell optoRight;
    SupremeHarmonicSheen harmonics;

    std::atomic<float> currentGrMeterDb { 0.0f };
    std::atomic<float> currentInMeterDb { -60.0f };
    std::atomic<float> currentOutMeterDb { -60.0f };
};
