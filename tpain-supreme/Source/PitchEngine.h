#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <cmath>
#include <array>
#include <algorithm>

namespace VeviAudio
{

enum class ScaleType
{
    Chromatic = 0,
    Major,
    NaturalMinor,
    HarmonicMinor,
    MelodicMinor,
    PentatonicMajor,
    PentatonicMinor,
    Blues
};

static const char* NoteNames[12] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

class PitchEngine
{
public:
    static constexpr int BUFFER_SIZE = 4096;
    static constexpr int BUFFER_MASK = BUFFER_SIZE - 1;
    static constexpr int MAX_WINDOW_SIZE = 1024;
    static constexpr int MAX_LAG = 1024;

    PitchEngine()
    {
        pitchBuffer.fill(0.0f);
        delayBufferL.fill(0.0f);
        delayBufferR.fill(0.0f);
        analysisWindow.fill(0.0f);
        nsdfBuffer.fill(0.0f);

        setSampleRate(44100.0);
        reset();
    }

    void setSampleRate(double newSampleRate) noexcept
    {
        if (newSampleRate > 1000.0 && newSampleRate < 384000.0)
            sampleRate = newSampleRate;
        else
            sampleRate = 44100.0;

        analysisWindowSize = (sampleRate >= 88200.0) ? 1024 : 512;
        if (analysisWindowSize > MAX_WINDOW_SIZE)
            analysisWindowSize = MAX_WINDOW_SIZE;

        maxDelaySamples = static_cast<int>(sampleRate * 0.007); // ~7ms
        if (maxDelaySamples < 128) maxDelaySamples = 128;
        if (maxDelaySamples > 1024) maxDelaySamples = 1024;

        reset();
    }

    void reset() noexcept
    {
        pitchBuffer.fill(0.0f);
        delayBufferL.fill(0.0f);
        delayBufferR.fill(0.0f);
        analysisWindow.fill(0.0f);
        nsdfBuffer.fill(0.0f);

        pitchBufferWritePos = 0;
        delayWritePos = 0;

        phase0 = 0.0;
        phase1 = 0.5;

        smoothedShiftSemitones = 0.0f;
        currentDetectedPitchHz = 0.0f;
        currentTargetPitchHz = 0.0f;
        currentDetectedMidi = 60.0f;
        currentTargetMidi = 60.0f;
        currentTransitionMidi = 60.0f;
        centsDeviation = 0.0f;
        isVoiced = false;
        confidence = 0.0f;
        hopCounter = 0;
    }

    void setRootNote(int root) noexcept
    {
        rootNote = juce::jlimit(0, 11, root);
    }

    void setScaleType(ScaleType type) noexcept
    {
        currentScale = type;
    }

    void setRetuneSpeedMs(float speedMs) noexcept
    {
        if (std::isfinite(speedMs))
            retuneSpeedMs = juce::jlimit(0.0f, 400.0f, speedMs);
        else
            retuneSpeedMs = 0.0f;
    }

    void setCentering(float centeringPercent) noexcept
    {
        if (std::isfinite(centeringPercent))
            centering = juce::jlimit(0.0f, 100.0f, centeringPercent) / 100.0f;
        else
            centering = 1.0f;
    }

    void setVariation(float variationPercent) noexcept
    {
        if (std::isfinite(variationPercent))
            variation = juce::jlimit(0.0f, 100.0f, variationPercent) / 100.0f;
        else
            variation = 0.0f;
    }

    void setTransitionMs(float transMs) noexcept
    {
        if (std::isfinite(transMs))
            transitionMs = juce::jlimit(0.0f, 250.0f, transMs);
        else
            transitionMs = 0.0f;
    }

    void setTrackingSensitivity(float sens) noexcept
    {
        if (std::isfinite(sens))
            trackingSensitivity = juce::jlimit(0.1f, 1.0f, sens);
        else
            trackingSensitivity = 0.85f;
    }

    // Getters for UI telemetry
    float getDetectedPitchHz() const noexcept { return currentDetectedPitchHz; }
    float getTargetPitchHz() const noexcept { return currentTargetPitchHz; }
    int getDetectedMidiNote() const noexcept { return static_cast<int>(std::round(currentDetectedMidi)); }
    int getTargetMidiNote() const noexcept { return static_cast<int>(std::round(currentTargetMidi)); }
    float getCentsDeviation() const noexcept { return centsDeviation; }
    bool getIsVoiced() const noexcept { return isVoiced; }
    float getConfidence() const noexcept { return confidence; }

    juce::String getDetectedNoteName() const
    {
        if (!isVoiced || currentDetectedPitchHz < 50.0f || !std::isfinite(currentDetectedPitchHz))
            return "--";
        int midi = getDetectedMidiNote();
        int noteIndex = (midi % 12 + 12) % 12;
        int octave = (midi / 12) - 1;
        return juce::String(NoteNames[noteIndex]) + juce::String(octave);
    }

    juce::String getTargetNoteName() const
    {
        if (!isVoiced || currentTargetPitchHz < 50.0f || !std::isfinite(currentTargetPitchHz))
            return "--";
        int midi = getTargetMidiNote();
        int noteIndex = (midi % 12 + 12) % 12;
        int octave = (midi / 12) - 1;
        return juce::String(NoteNames[noteIndex]) + juce::String(octave);
    }

    int getLatencySamples() const noexcept
    {
        return maxDelaySamples / 2;
    }

    static uint16_t getScaleMask(ScaleType scale) noexcept
    {
        switch (scale)
        {
            case ScaleType::Chromatic:
                return 0x0FFF;
            case ScaleType::Major:
                return (1 << 0) | (1 << 2) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11);
            case ScaleType::NaturalMinor:
                return (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 8) | (1 << 10);
            case ScaleType::HarmonicMinor:
                return (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 8) | (1 << 11);
            case ScaleType::MelodicMinor:
                return (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11);
            case ScaleType::PentatonicMajor:
                return (1 << 0) | (1 << 2) | (1 << 4) | (1 << 7) | (1 << 9);
            case ScaleType::PentatonicMinor:
                return (1 << 0) | (1 << 3) | (1 << 5) | (1 << 7) | (1 << 10);
            case ScaleType::Blues:
                return (1 << 0) | (1 << 3) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 10);
            default:
                return 0x0FFF;
        }
    }

    void process(const float* inL, const float* inR, float* outL, float* outR, int numSamples) noexcept
    {
        if (numSamples <= 0 || inL == nullptr || outL == nullptr)
            return;

        const int hopSize = 64;

        for (int i = 0; i < numSamples; ++i)
        {
            float sL = inL[i];
            if (!std::isfinite(sL)) sL = 0.0f;

            float sR = sL;
            if (inR != nullptr)
            {
                sR = inR[i];
                if (!std::isfinite(sR)) sR = 0.0f;
            }

            float monoIn = 0.5f * (sL + sR);

            // Feed circular buffer using bitmask
            pitchBuffer[pitchBufferWritePos & BUFFER_MASK] = monoIn;
            pitchBufferWritePos = (pitchBufferWritePos + 1) & BUFFER_MASK;

            // Periodic pitch detection
            if (++hopCounter >= hopSize)
            {
                hopCounter = 0;
                detectAndQuantizePitch();
            }

            // Note Transition Glide
            if (transitionMs <= 0.5f)
            {
                currentTransitionMidi = currentTargetMidi;
            }
            else
            {
                float dtTrans = 1.0f / static_cast<float>(sampleRate);
                float tauTrans = transitionMs * 0.001f;
                float alphaTrans = 1.0f - std::exp(-dtTrans / (tauTrans + 1e-6f));
                currentTransitionMidi += alphaTrans * (currentTargetMidi - currentTransitionMidi);
            }

            // Calculate raw correction shift with Centering
            float targetShift = 0.0f;
            if (isVoiced && confidence > 0.45f)
            {
                targetShift = (currentTransitionMidi - currentDetectedMidi) * centering;

                if (variation > 0.001f)
                {
                    float naturalVibrato = currentDetectedMidi - std::round(currentDetectedMidi);
                    targetShift += naturalVibrato * variation;
                }
            }

            if (!std::isfinite(targetShift))
                targetShift = 0.0f;

            targetShift = juce::jlimit(-12.0f, 12.0f, targetShift);

            if (retuneSpeedMs <= 0.5f)
            {
                smoothedShiftSemitones = targetShift;
            }
            else
            {
                float dt = 1.0f / static_cast<float>(sampleRate);
                float tau = retuneSpeedMs * 0.001f;
                float alpha = 1.0f - std::exp(-dt / (tau + 1e-6f));
                smoothedShiftSemitones += alpha * (targetShift - smoothedShiftSemitones);
            }

            if (!std::isfinite(smoothedShiftSemitones))
                smoothedShiftSemitones = 0.0f;

            float pitchRatio = std::pow(2.0f, smoothedShiftSemitones / 12.0f);
            if (!std::isfinite(pitchRatio))
                pitchRatio = 1.0f;
            pitchRatio = juce::jlimit(0.5f, 2.0f, pitchRatio);

            // Time-Domain Pitch Shifter
            delayBufferL[delayWritePos & BUFFER_MASK] = sL;
            delayBufferR[delayWritePos & BUFFER_MASK] = sR;

            float grainPeriod = static_cast<float>(maxDelaySamples);
            float rate = (1.0f - pitchRatio) / (grainPeriod + 1e-6f);

            phase0 += rate;
            if (phase0 >= 1.0) phase0 -= std::floor(phase0);
            if (phase0 < 0.0) phase0 += (1.0 - std::floor(phase0));
            if (!std::isfinite(phase0)) phase0 = 0.0;

            phase1 = phase0 + 0.5;
            if (phase1 >= 1.0) phase1 -= 1.0;
            if (!std::isfinite(phase1)) phase1 = 0.5;

            float w0 = 0.5f * (1.0f - std::cos(juce::MathConstants<float>::twoPi * static_cast<float>(phase0)));
            float w1 = 0.5f * (1.0f - std::cos(juce::MathConstants<float>::twoPi * static_cast<float>(phase1)));

            float d0 = static_cast<float>(phase0) * grainPeriod;
            float d1 = static_cast<float>(phase1) * grainPeriod;

            float shiftedL = w0 * readInterpolated(delayBufferL.data(), static_cast<float>(delayWritePos) - d0)
                           + w1 * readInterpolated(delayBufferL.data(), static_cast<float>(delayWritePos) - d1);

            float shiftedR = w0 * readInterpolated(delayBufferR.data(), static_cast<float>(delayWritePos) - d0)
                           + w1 * readInterpolated(delayBufferR.data(), static_cast<float>(delayWritePos) - d1);

            float wetWeight = isVoiced ? juce::jlimit(0.0f, 1.0f, confidence * 1.5f) : 0.0f;
            float dryAlignedL = readInterpolated(delayBufferL.data(), static_cast<float>(delayWritePos) - (grainPeriod * 0.5f));
            float dryAlignedR = readInterpolated(delayBufferR.data(), static_cast<float>(delayWritePos) - (grainPeriod * 0.5f));

            float finalL = wetWeight * shiftedL + (1.0f - wetWeight) * dryAlignedL;
            float finalR = wetWeight * shiftedR + (1.0f - wetWeight) * dryAlignedR;

            outL[i] = std::isfinite(finalL) ? finalL : sL;
            if (outR != nullptr)
                outR[i] = std::isfinite(finalR) ? finalR : sR;

            delayWritePos = (delayWritePos + 1) & BUFFER_MASK;
        }
    }

private:
    double sampleRate = 44100.0;
    int analysisWindowSize = 512;
    int maxDelaySamples = 256;

    // Fixed pre-allocated buffers (Zero Heap Allocations on Audio Thread)
    std::array<float, BUFFER_SIZE> pitchBuffer;
    int pitchBufferWritePos = 0;

    std::array<float, BUFFER_SIZE> delayBufferL;
    std::array<float, BUFFER_SIZE> delayBufferR;
    int delayWritePos = 0;

    std::array<float, MAX_WINDOW_SIZE> analysisWindow;
    std::array<float, MAX_LAG> nsdfBuffer;

    double phase0 = 0.0;
    double phase1 = 0.5;

    int rootNote = 0;
    ScaleType currentScale = ScaleType::Major;
    float retuneSpeedMs = 0.0f;
    float centering = 1.0f;
    float variation = 0.0f;
    float transitionMs = 0.0f;
    float currentTransitionMidi = 60.0f;
    float trackingSensitivity = 0.85f;

    float currentDetectedPitchHz = 0.0f;
    float currentTargetPitchHz = 0.0f;
    float currentDetectedMidi = 60.0f;
    float currentTargetMidi = 60.0f;
    float centsDeviation = 0.0f;
    float smoothedShiftSemitones = 0.0f;
    bool isVoiced = false;
    float confidence = 0.0f;
    int hopCounter = 0;

    // Crash-proof Cubic Hermite interpolation using bitmask & fmod
    inline float readInterpolated(const float* buf, float readPos) const noexcept
    {
        if (!std::isfinite(readPos) || buf == nullptr)
            return 0.0f;

        readPos = std::fmod(readPos, static_cast<float>(BUFFER_SIZE));
        if (readPos < 0.0f)
            readPos += static_cast<float>(BUFFER_SIZE);

        int i1 = static_cast<int>(readPos) & BUFFER_MASK;
        float frac = readPos - static_cast<float>(i1);

        int i0 = (i1 - 1) & BUFFER_MASK;
        int i2 = (i1 + 1) & BUFFER_MASK;
        int i3 = (i1 + 2) & BUFFER_MASK;

        float y0 = buf[i0];
        float y1 = buf[i1];
        float y2 = buf[i2];
        float y3 = buf[i3];

        float c0 = y1;
        float c1 = 0.5f * (y2 - y0);
        float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

        float res = ((c3 * frac + c2) * frac + c1) * frac + c0;
        return std::isfinite(res) ? res : 0.0f;
    }

    void detectAndQuantizePitch() noexcept
    {
        const int N = analysisWindowSize;
        if (N <= 0 || N > MAX_WINDOW_SIZE)
        {
            isVoiced = false;
            return;
        }

        // Copy recent window without heap allocation
        float energy = 0.0f;
        for (int i = 0; i < N; ++i)
        {
            int idx = (pitchBufferWritePos - N + i) & BUFFER_MASK;
            float val = pitchBuffer[idx];
            analysisWindow[i] = val;
            energy += val * val;
        }

        if (!std::isfinite(energy) || energy < 1e-8f)
        {
            isVoiced = false;
            confidence = 0.0f;
            centsDeviation = 0.0f;
            return;
        }

        float rms = std::sqrt(energy / static_cast<float>(N));
        if (rms < 0.003f) // Silence gate (~ -50 dBFS)
        {
            isVoiced = false;
            confidence = 0.0f;
            centsDeviation = 0.0f;
            return;
        }

        int minLag = static_cast<int>(sampleRate / 1100.0);
        int maxLag = static_cast<int>(sampleRate / 65.0);
        if (minLag < 2) minLag = 2;
        if (maxLag >= N - 1) maxLag = N - 2;
        if (maxLag >= MAX_LAG - 1) maxLag = MAX_LAG - 2;

        if (maxLag <= minLag + 1)
        {
            isVoiced = false;
            return;
        }

        // Compute NSDF into pre-allocated array
        for (int tau = minLag; tau <= maxLag; ++tau)
        {
            float acf = 0.0f;
            float m = 0.0f;
            int limit = N - tau;
            for (int j = 0; j < limit; ++j)
            {
                float x1 = analysisWindow[j];
                float x2 = analysisWindow[j + tau];
                acf += x1 * x2;
                m += x1 * x1 + x2 * x2;
            }
            nsdfBuffer[tau] = (m > 1e-9f) ? (2.0f * acf / m) : 0.0f;
        }

        // Peak search
        int bestTau = -1;
        float maxVal = -1.0f;
        float threshold = 0.55f * trackingSensitivity;

        for (int tau = minLag + 1; tau < maxLag; ++tau)
        {
            if (nsdfBuffer[tau] > nsdfBuffer[tau - 1] && nsdfBuffer[tau] >= nsdfBuffer[tau + 1])
            {
                if (nsdfBuffer[tau] > threshold)
                {
                    bestTau = tau;
                    maxVal = nsdfBuffer[tau];
                    break;
                }
            }
        }

        if (bestTau > minLag && bestTau < maxLag && maxVal > 0.45f)
        {
            float y0 = nsdfBuffer[bestTau - 1];
            float y1 = nsdfBuffer[bestTau];
            float y2 = nsdfBuffer[bestTau + 1];
            float denom = (y0 - 2.0f * y1 + y2);
            float delta = (std::abs(denom) > 1e-7f) ? (0.5f * (y0 - y2) / denom) : 0.0f;

            float exactTau = static_cast<float>(bestTau) + delta;
            if (exactTau > 1.0f)
            {
                float detectedHz = static_cast<float>(sampleRate) / exactTau;
                if (std::isfinite(detectedHz) && detectedHz >= 60.0f && detectedHz <= 1200.0f)
                {
                    currentDetectedPitchHz = detectedHz;
                    confidence = juce::jlimit(0.0f, 1.0f, maxVal);
                    isVoiced = true;

                    currentDetectedMidi = 69.0f + 12.0f * std::log2(std::max(1.0f, currentDetectedPitchHz) / 440.0f);
                    currentTargetMidi = quantizeToScale(currentDetectedMidi);
                    currentTargetPitchHz = 440.0f * std::pow(2.0f, (currentTargetMidi - 69.0f) / 12.0f);

                    centsDeviation = (currentTargetMidi - currentDetectedMidi) * 100.0f;
                    return;
                }
            }
        }

        isVoiced = false;
        confidence = 0.0f;
        centsDeviation = 0.0f;
    }

    float quantizeToScale(float inputMidi) const noexcept
    {
        if (!std::isfinite(inputMidi))
            return 60.0f;

        uint16_t scaleMask = getScaleMask(currentScale);
        int baseMidi = static_cast<int>(std::round(inputMidi));
        int bestNote = baseMidi;
        float bestDist = 999.0f;

        for (int offset = -6; offset <= 6; ++offset)
        {
            int testNote = baseMidi + offset;
            int semitoneInOctave = ((testNote % 12) + 12) % 12;
            int semitoneRelRoot = ((semitoneInOctave - rootNote) % 12 + 12) % 12;

            if ((scaleMask & (1 << semitoneRelRoot)) != 0)
            {
                float dist = std::abs(inputMidi - static_cast<float>(testNote));
                if (dist < bestDist)
                {
                    bestDist = dist;
                    bestNote = testNote;
                }
            }
        }

        return static_cast<float>(bestNote);
    }
};

} // namespace VeviAudio
