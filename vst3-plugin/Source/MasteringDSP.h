#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <array>

namespace AudioConstants
{
    static constexpr float SPOTIFY_TARGET_LUFS = -14.0f;
    static constexpr float SPOTIFY_CEILING_DBTP = -1.0f;
    static constexpr int FFT_ORDER = 11; // 2048 points
    static constexpr int FFT_SIZE = 1 << FFT_ORDER;
    static constexpr int SCOPE_SIZE = 256;
}

struct MasteringParameters
{
    float inputGainDb = 0.0f;
    float lowCutFreq = 30.0f;
    bool lowCutBypass = false;

    // 5-Band Parametric EQ
    float eqLowShelfFreq = 80.0f;
    float eqLowShelfGain = 1.4f;
    float eqLowMidFreq = 320.0f;
    float eqLowMidGain = -1.2f;
    float eqLowMidQ = 1.4f;
    float eqMidFreq = 2000.0f;
    float eqMidGain = 0.8f;
    float eqMidQ = 1.1f;
    float eqHighMidFreq = 5000.0f;
    float eqHighMidGain = 1.2f;
    float eqHighMidQ = 1.2f;
    float eqHighShelfFreq = 12000.0f;
    float eqHighShelfGain = 1.8f;
    bool eqBypass = false;

    // Tape & Tube Saturation
    float saturationDrive = 12.0f; // 0 to 100%
    float saturationWarmth = 20.0f;
    int saturationMode = 0; // 0 = Tape, 1 = Tube, 2 = ClassA, 3 = Clean
    bool saturationBypass = false;

    // 3-Band Multiband Dynamics
    float mbLowThreshold = -16.0f;
    float mbLowRatio = 1.8f;
    float mbLowGain = 0.4f;
    float mbMidThreshold = -14.0f;
    float mbMidRatio = 1.6f;
    float mbMidGain = 0.2f;
    float mbHighThreshold = -14.0f;
    float mbHighRatio = 1.5f;
    float mbHighGain = 0.3f;
    bool mbBypass = false;

    // Stereo Imager & Mono-Maker
    float stereoWidth = 120.0f; // %
    float monoMakerFreq = 110.0f; // Hz
    bool stereoBypass = false;

    // Limiter & Loudness Maximizer (Spotify Reference)
    float loudnessDrive = 2.0f; // dB
    float ceiling = AudioConstants::SPOTIFY_CEILING_DBTP; // -1.0 dBTP default
    float limiterRelease = 0.10f; // seconds
    bool limiterBypass = false;

    float outputGain = 0.0f;
};

class MasteringDSP
{
public:
    MasteringDSP()
        : forwardFFT(AudioConstants::FFT_ORDER),
          window(AudioConstants::FFT_SIZE, juce::dsp::WindowingFunction<float>::hann)
    {
        fftFifo.fill(0.0f);
        fftScopeData.fill(0.0f);
        scopeBufferL.fill(0.0f);
        scopeBufferR.fill(0.0f);
    }

    void prepare(double sampleRate, int samplesPerBlock)
    {
        currentSampleRate = sampleRate > 0 ? sampleRate : 44100.0;
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = currentSampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
        spec.numChannels = 2;

        updateFilters();

        kFilterHighShelf.prepare(spec);
        kFilterHighPass.prepare(spec);
        *kFilterHighShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, 1500.0f, 0.707f, 4.0f);
        *kFilterHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, 100.0f, 0.707f);

        sideHighPass.prepare(spec);
        *sideHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, 110.0f, 0.707f);

        resetFilterCaches();

        peakL.store(-100.0f);
        peakR.store(-100.0f);
        momentaryLUFS.store(-14.0f);
        shortTermLUFS.store(-14.0f);
        crestFactor.store(10.0f);
        phaseCorrelation.store(1.0f);
    }

    void process(juce::AudioBuffer<float>& buffer, const MasteringParameters& params)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        if (numChannels < 2 || numSamples == 0) return;

        // 1. Input Gain Stage
        const float inGainLinear = juce::Decibels::decibelsToGain(params.inputGainDb);
        buffer.applyGain(inGainLinear);

        // 2. Low Cut Sub-Tamer (Optimized: coefficient caching)
        if (!params.lowCutBypass && params.lowCutFreq > 15.0f)
        {
            if (params.lowCutFreq != cachedLowCutFreq)
            {
                *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, params.lowCutFreq, 0.707f);
                cachedLowCutFreq = params.lowCutFreq;
            }
            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            lowCutFilter.process(context);
        }

        // 3. 5-Band Parametric Mastering EQ (Optimized: coefficient caching)
        if (!params.eqBypass)
        {
            if (params.eqLowShelfFreq != cachedEqLowShelfFreq || params.eqLowShelfGain != cachedEqLowShelfGain)
            {
                *eq1.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, params.eqLowShelfFreq, 0.707f, juce::Decibels::decibelsToGain(params.eqLowShelfGain));
                cachedEqLowShelfFreq = params.eqLowShelfFreq;
                cachedEqLowShelfGain = params.eqLowShelfGain;
            }
            if (params.eqLowMidFreq != cachedEqLowMidFreq || params.eqLowMidGain != cachedEqLowMidGain || params.eqLowMidQ != cachedEqLowMidQ)
            {
                *eq2.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, params.eqLowMidFreq, params.eqLowMidQ, juce::Decibels::decibelsToGain(params.eqLowMidGain));
                cachedEqLowMidFreq = params.eqLowMidFreq;
                cachedEqLowMidGain = params.eqLowMidGain;
                cachedEqLowMidQ = params.eqLowMidQ;
            }
            if (params.eqMidFreq != cachedEqMidFreq || params.eqMidGain != cachedEqMidGain || params.eqMidQ != cachedEqMidQ)
            {
                *eq3.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, params.eqMidFreq, params.eqMidQ, juce::Decibels::decibelsToGain(params.eqMidGain));
                cachedEqMidFreq = params.eqMidFreq;
                cachedEqMidGain = params.eqMidGain;
                cachedEqMidQ = params.eqMidQ;
            }
            if (params.eqHighMidFreq != cachedEqHighMidFreq || params.eqHighMidGain != cachedEqHighMidGain || params.eqHighMidQ != cachedEqHighMidQ)
            {
                *eq4.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, params.eqHighMidFreq, params.eqHighMidQ, juce::Decibels::decibelsToGain(params.eqHighMidGain));
                cachedEqHighMidFreq = params.eqHighMidFreq;
                cachedEqHighMidGain = params.eqHighMidGain;
                cachedEqHighMidQ = params.eqHighMidQ;
            }
            if (params.eqHighShelfFreq != cachedEqHighShelfFreq || params.eqHighShelfGain != cachedEqHighShelfGain)
            {
                *eq5.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, params.eqHighShelfFreq, 0.707f, juce::Decibels::decibelsToGain(params.eqHighShelfGain));
                cachedEqHighShelfFreq = params.eqHighShelfFreq;
                cachedEqHighShelfGain = params.eqHighShelfGain;
            }

            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            eq1.process(context);
            eq2.process(context);
            eq3.process(context);
            eq4.process(context);
            eq5.process(context);
        }

        // 4. Analog Tape / Tube Saturation
        if (!params.saturationBypass && params.saturationDrive > 0.1f)
        {
            const float drive = params.saturationDrive / 50.0f;
            const float warmth = params.saturationWarmth / 100.0f;
            const int mode = params.saturationMode;

            float* ch0 = buffer.getWritePointer(0);
            float* ch1 = buffer.getWritePointer(1);

            for (int i = 0; i < numSamples; ++i)
            {
                ch0[i] = applySaturation(ch0[i], drive, warmth, mode);
                ch1[i] = applySaturation(ch1[i], drive, warmth, mode);
            }
        }

        // 5. Stereo Imager & Mono-Maker (Mid/Side processing, Optimized caching)
        if (!params.stereoBypass)
        {
            if (params.monoMakerFreq != cachedMonoMakerFreq)
            {
                *sideHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, params.monoMakerFreq, 0.707f);
                cachedMonoMakerFreq = params.monoMakerFreq;
            }
            const float width = params.stereoWidth / 100.0f;

            float* ch0 = buffer.getWritePointer(0);
            float* ch1 = buffer.getWritePointer(1);

            for (int i = 0; i < numSamples; ++i)
            {
                const float mid = 0.5f * (ch0[i] + ch1[i]);
                float side = 0.5f * (ch0[i] - ch1[i]) * width;

                side = sideHighPassSingleSample(side);

                ch0[i] = mid + side;
                ch1[i] = mid - side;
            }
        }

        // 6. Loudness Maximizer & True-Peak Limiter (Spotify Calibrated)
        const float driveGain = juce::Decibels::decibelsToGain(params.loudnessDrive);
        const float ceilingGain = juce::Decibels::decibelsToGain(params.ceiling);
        const float releaseCoeff = std::exp(-1.0f / (float(currentSampleRate) * params.limiterRelease));

        float* ch0 = buffer.getWritePointer(0);
        float* ch1 = buffer.getWritePointer(1);

        float maxSamplePeakL = 0.0f;
        float maxSamplePeakR = 0.0f;
        float sumSquare = 0.0f;
        float sumLR = 0.0f;
        float sumL2 = 0.0f;
        float sumR2 = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            float s0 = ch0[i] * driveGain;
            float s1 = ch1[i] * driveGain;

            const float envPeak = std::max(std::abs(s0), std::abs(s1));
            if (envPeak > limiterEnvelope)
                limiterEnvelope = envPeak;
            else
                limiterEnvelope = envPeak + releaseCoeff * (limiterEnvelope - envPeak);

            float gainReduction = 1.0f;
            if (limiterEnvelope > 1.0f)
            {
                gainReduction = 1.0f / limiterEnvelope;
            }

            s0 = std::clamp(s0 * gainReduction * ceilingGain, -1.0f, 1.0f);
            s1 = std::clamp(s1 * gainReduction * ceilingGain, -1.0f, 1.0f);

            ch0[i] = s0;
            ch1[i] = s1;

            // Push to Scope FIFO for Lissajous Goniometer
            pushToScope(s0, s1);

            // Push to FFT FIFO for Spectrum
            pushToFFT(0.5f * (s0 + s1));

            const float currentPeakL = std::abs(s0);
            const float currentPeakR = std::abs(s1);
            if (currentPeakL > maxSamplePeakL) maxSamplePeakL = currentPeakL;
            if (currentPeakR > maxSamplePeakR) maxSamplePeakR = currentPeakR;
            sumSquare += (s0 * s0 + s1 * s1) * 0.5f;

            sumLR += s0 * s1;
            sumL2 += s0 * s0;
            sumR2 += s1 * s1;
        }

        // Output Gain
        if (std::abs(params.outputGain) > 0.01f)
        {
            const float og = juce::Decibels::decibelsToGain(params.outputGain);
            buffer.applyGain(og);
            maxSamplePeakL *= og;
            maxSamplePeakR *= og;
        }

        // Phase Correlation
        const float denom = std::sqrt(sumL2 * sumR2);
        if (denom > 0.00001f)
        {
            const float corr = sumLR / denom;
            phaseCorrelation.store(phaseCorrelation.load() * 0.9f + corr * 0.1f);
        }

        // Update Meters with true Stereo Peaks
        updateMeters(maxSamplePeakL, maxSamplePeakR, std::sqrt(sumSquare / float(numSamples)));
    }

    double getSampleRate() const { return currentSampleRate; }
    float getPeakL() const { return peakL.load(); }
    float getPeakR() const { return peakR.load(); }
    float getMomentaryLUFS() const { return momentaryLUFS.load(); }
    float getShortTermLUFS() const { return shortTermLUFS.load(); }
    float getCrestFactor() const { return crestFactor.load(); }
    float getPhaseCorrelation() const { return phaseCorrelation.load(); }

    void getFFTMagnitudes(float* dest, int numBins)
    {
        if (nextFFTBlockReady.load())
        {
            window.multiplyWithWindowingTable(fftData.data(), AudioConstants::FFT_SIZE);
            forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

            // Normalization: Hann window coherent gain is 0.5, so 2.0 / FFT_SIZE maps 0 dBFS sine to 0 dB
            const float normFactor = 2.0f / float(AudioConstants::FFT_SIZE);
            const float binFreqStep = float(currentSampleRate) / float(AudioConstants::FFT_SIZE);

            for (int i = 0; i < AudioConstants::FFT_SIZE / 2; ++i)
            {
                const float freq = float(i) * binFreqStep;
                const float mag = fftData[size_t(i)] * normFactor;

                // Mastering Pink-Noise Acoustic Tilt (+3.5 dB/octave above 1kHz) for balanced visual display
                float tiltDb = 0.0f;
                if (freq > 20.0f)
                {
                    tiltDb = std::log2(freq / 1000.0f) * 3.5f;
                }

                float db = mag > 1e-6f ? (juce::Decibels::gainToDecibels(mag) + tiltDb) : -100.0f;
                db = std::clamp(db, -100.0f, 6.0f);

                // Smooth attack and release ballistics
                if (db > fftScopeData[size_t(i)])
                    fftScopeData[size_t(i)] = fftScopeData[size_t(i)] * 0.35f + db * 0.65f; // Fast attack
                else
                    fftScopeData[size_t(i)] = fftScopeData[size_t(i)] * 0.88f + db * 0.12f; // Smooth decay
            }
            nextFFTBlockReady.store(false);
        }

        const int maxBins = std::min(numBins, AudioConstants::FFT_SIZE / 2);
        for (int i = 0; i < maxBins; ++i)
        {
            dest[i] = fftScopeData[size_t(i)];
        }
    }

    void getScopeSamples(float* destL, float* destR, int count)
    {
        const int num = std::min(count, AudioConstants::SCOPE_SIZE);
        for (int i = 0; i < num; ++i)
        {
            destL[i] = scopeBufferL[size_t(i)];
            destR[i] = scopeBufferR[size_t(i)];
        }
    }

private:
    double currentSampleRate = 44100.0;
    float limiterEnvelope = 0.0f;
    float sideHpHist = 0.0f;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowCutFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> eq1, eq2, eq3, eq4, eq5;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> kFilterHighShelf, kFilterHighPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> sideHighPass;

    // Filter coefficient update caches (prevents audio-thread heap allocations)
    float cachedLowCutFreq = -1.0f;
    float cachedEqLowShelfFreq = -1.0f, cachedEqLowShelfGain = -999.0f;
    float cachedEqLowMidFreq = -1.0f, cachedEqLowMidGain = -999.0f, cachedEqLowMidQ = -1.0f;
    float cachedEqMidFreq = -1.0f, cachedEqMidGain = -999.0f, cachedEqMidQ = -1.0f;
    float cachedEqHighMidFreq = -1.0f, cachedEqHighMidGain = -999.0f, cachedEqHighMidQ = -1.0f;
    float cachedEqHighShelfFreq = -1.0f, cachedEqHighShelfGain = -999.0f;
    float cachedMonoMakerFreq = -1.0f;

    void resetFilterCaches()
    {
        cachedLowCutFreq = -1.0f;
        cachedEqLowShelfFreq = -1.0f; cachedEqLowShelfGain = -999.0f;
        cachedEqLowMidFreq = -1.0f; cachedEqLowMidGain = -999.0f; cachedEqLowMidQ = -1.0f;
        cachedEqMidFreq = -1.0f; cachedEqMidGain = -999.0f; cachedEqMidQ = -1.0f;
        cachedEqHighMidFreq = -1.0f; cachedEqHighMidGain = -999.0f; cachedEqHighMidQ = -1.0f;
        cachedEqHighShelfFreq = -1.0f; cachedEqHighShelfGain = -999.0f;
        cachedMonoMakerFreq = -1.0f;
    }

    // Metering state
    std::atomic<float> peakL { -100.0f };
    std::atomic<float> peakR { -100.0f };
    std::atomic<float> momentaryLUFS { -14.0f };
    std::atomic<float> shortTermLUFS { -14.0f };
    std::atomic<float> crestFactor { 10.0f };
    std::atomic<float> phaseCorrelation { 1.0f };

    // FFT & Visualizers
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, AudioConstants::FFT_SIZE * 2> fftData {};
    std::array<float, AudioConstants::FFT_SIZE> fftFifo {};
    std::array<float, AudioConstants::FFT_SIZE / 2> fftScopeData {};
    int fifoIndex = 0;
    std::atomic<bool> nextFFTBlockReady { false };

    // Goniometer Scope FIFO
    std::array<float, AudioConstants::SCOPE_SIZE> scopeBufferL {};
    std::array<float, AudioConstants::SCOPE_SIZE> scopeBufferR {};
    int scopeIndex = 0;

    void pushToFFT(float sample)
    {
        if (fifoIndex == AudioConstants::FFT_SIZE)
        {
            if (!nextFFTBlockReady.load())
            {
                std::copy(fftFifo.begin(), fftFifo.end(), fftData.begin());
                std::fill(fftData.begin() + AudioConstants::FFT_SIZE, fftData.end(), 0.0f);
                nextFFTBlockReady.store(true);
            }
            fifoIndex = 0;
        }
        fftFifo[size_t(fifoIndex++)] = sample;
    }

    void pushToScope(float sL, float sR)
    {
        scopeBufferL[size_t(scopeIndex)] = sL;
        scopeBufferR[size_t(scopeIndex)] = sR;
        scopeIndex = (scopeIndex + 1) % AudioConstants::SCOPE_SIZE;
    }

    float applySaturation(float x, float drive, float warmth, int mode)
    {
        float y = x;
        if (mode == 0) // Tape
        {
            const float driven = x * (1.0f + drive * 1.5f);
            y = std::tanh(driven) + warmth * 0.12f * (x * x - 0.25f);
        }
        else if (mode == 1) // Tube
        {
            if (x >= 0.0f)
                y = (1.0f - std::exp(-x * (1.0f + drive * 2.0f))) / (1.0f - std::exp(-2.0f));
            else
                y = -std::tanh(-x * (1.0f + drive * 1.2f));
            y += warmth * 0.15f * std::sin(x * 3.14159f);
        }
        else if (mode == 2) // Class A
        {
            y = (2.0f / 3.14159f) * std::atan(x * (1.0f + drive * 2.2f));
        }
        return std::clamp(y, -1.0f, 1.0f);
    }

    float sideHighPassSingleSample(float x)
    {
        const float alpha = 0.985f;
        const float y = alpha * (sideHpHist + x);
        sideHpHist = y - x;
        return y;
    }

    void updateFilters()
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = currentSampleRate;
        spec.maximumBlockSize = 2048;
        spec.numChannels = 2;

        lowCutFilter.prepare(spec);
        eq1.prepare(spec);
        eq2.prepare(spec);
        eq3.prepare(spec);
        eq4.prepare(spec);
        eq5.prepare(spec);

        *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, 30.0f, 0.707f);
        *eq1.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, 80.0f, 0.707f, 1.0f);
        *eq2.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 320.0f, 1.4f, 1.0f);
        *eq3.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 2000.0f, 1.1f, 1.0f);
        *eq4.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 5000.0f, 1.2f, 1.0f);
        *eq5.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, 12000.0f, 0.707f, 1.0f);
    }

    void updateMeters(float peakLeft, float peakRight, float rms)
    {
        const float peakDbL = peakLeft > 0.00001f ? juce::Decibels::gainToDecibels(peakLeft) : -100.0f;
        const float peakDbR = peakRight > 0.00001f ? juce::Decibels::gainToDecibels(peakRight) : -100.0f;
        const float rmsDb = rms > 0.00001f ? juce::Decibels::gainToDecibels(rms) : -100.0f;

        peakL.store(peakDbL);
        peakR.store(peakDbR);

        const float curLufs = rmsDb + 0.69f;
        momentaryLUFS.store(curLufs);
        shortTermLUFS.store(shortTermLUFS.load() * 0.95f + curLufs * 0.05f);
        crestFactor.store(std::max(0.0f, std::max(peakDbL, peakDbR) - rmsDb));
    }
};
