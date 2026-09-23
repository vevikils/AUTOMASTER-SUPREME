#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace VeviAudio
{

class VocalFXDSP
{
public:
    VocalFXDSP() = default;

    void prepare(double newSampleRate, int samplesPerBlock)
    {
        if (newSampleRate > 1000.0 && newSampleRate < 384000.0)
            sampleRate = newSampleRate;
        else
            sampleRate = 44100.0;

        int safeBlockSize = 65536;

        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(safeBlockSize);
        spec.numChannels = 2;

        lowCutFilter.prepare(spec);
        bodyFilter.prepare(spec);
        airFilter.prepare(spec);

        reverbProcessor.prepare(spec);
        updateReverbParameters();

        // Pre-allocate reverb buffer safely up to maxBlockSize
        reverbBuffer.setSize(2, safeBlockSize, false, true, true);
        reverbBuffer.clear();

        // 2-second stereo delay buffer
        int maxDelayLen = static_cast<int>(sampleRate * 2.0);
        if (maxDelayLen < 4096) maxDelayLen = 4096;
        delayLineL.assign(maxDelayLen, 0.0f);
        delayLineR.assign(maxDelayLen, 0.0f);
        delayWriteIdx = 0;

        reset();
    }

    void reset() noexcept
    {
        lowCutFilter.reset();
        bodyFilter.reset();
        airFilter.reset();
        reverbProcessor.reset();

        std::fill(delayLineL.begin(), delayLineL.end(), 0.0f);
        std::fill(delayLineR.begin(), delayLineR.end(), 0.0f);
        delayWriteIdx = 0;
        delayDampedL = 0.0f;
        delayDampedR = 0.0f;

        envFastL = 0.0f;
        envSlowL = 0.0f;
        envFastR = 0.0f;
        envSlowR = 0.0f;

        compEnvelope = 0.0f;
        gainReductionDb = 0.0f;
    }

    void setTransientAmount(float amount) noexcept
    {
        if (std::isfinite(amount))
            transientAmount = juce::jlimit(-100.0f, 100.0f, amount);
        else
            transientAmount = 0.0f;
    }

    void setCompression(float amountPercent) noexcept
    {
        if (std::isfinite(amountPercent))
            compAmount = juce::jlimit(0.0f, 100.0f, amountPercent) / 100.0f;
        else
            compAmount = 0.0f;
    }

    void setLowCut(float freqHz) noexcept
    {
        if (std::isfinite(freqHz))
            lowCutFreq = juce::jlimit(20.0f, 250.0f, freqHz);
        else
            lowCutFreq = 80.0f;
        updateFilters();
    }

    void setBodyGain(float gainDb) noexcept
    {
        if (std::isfinite(gainDb))
            bodyGainDb = juce::jlimit(-6.0f, 6.0f, gainDb);
        else
            bodyGainDb = 0.0f;
        updateFilters();
    }

    void setAirGain(float gainDb) noexcept
    {
        if (std::isfinite(gainDb))
            airGainDb = juce::jlimit(0.0f, 10.0f, gainDb);
        else
            airGainDb = 3.0f;
        updateFilters();
    }

    void setReverbMix(float percent) noexcept
    {
        if (std::isfinite(percent))
            reverbMix = juce::jlimit(0.0f, 100.0f, percent) / 100.0f;
        else
            reverbMix = 0.0f;
    }

    void setReverbSize(float percent) noexcept
    {
        if (std::isfinite(percent))
            reverbSize = juce::jlimit(0.0f, 100.0f, percent) / 100.0f;
        else
            reverbSize = 0.5f;
        updateReverbParameters();
    }

    void setDelayMix(float percent) noexcept
    {
        if (std::isfinite(percent))
            delayMix = juce::jlimit(0.0f, 100.0f, percent) / 100.0f;
        else
            delayMix = 0.0f;
    }

    void setDelayTimeMs(float ms) noexcept
    {
        if (std::isfinite(ms))
            delayTimeMs = juce::jlimit(20.0f, 1000.0f, ms);
        else
            delayTimeMs = 320.0f;
    }

    void setDelayFeedback(float percent) noexcept
    {
        if (std::isfinite(percent))
            delayFeedback = juce::jlimit(0.0f, 85.0f, percent) / 100.0f;
        else
            delayFeedback = 0.35f;
    }

    void setOutputGainDb(float gainDb) noexcept
    {
        if (std::isfinite(gainDb))
            outputGain = juce::Decibels::decibelsToGain(juce::jlimit(-24.0f, 24.0f, gainDb));
        else
            outputGain = 1.0f;
    }

    void setMix(float mixPercent) noexcept
    {
        if (std::isfinite(mixPercent))
            mix = juce::jlimit(0.0f, 100.0f, mixPercent) / 100.0f;
        else
            mix = 1.0f;
    }

    float getGainReductionDb() const noexcept { return gainReductionDb; }
    float getTransientActivity() const noexcept { return transientActivity; }

    void process(juce::AudioBuffer<float>& buffer) noexcept
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        if (numChannels <= 0 || numSamples <= 0) return;

        // 1. Transient Shaper
        processTransients(buffer);

        // 2. Vocal Compressor
        processCompression(buffer);

        // 3. Light Musical EQ (Low-Cut, Body, High Air)
        try
        {
            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);

            lowCutFilter.process(context);
            bodyFilter.process(context);
            airFilter.process(context);
        }
        catch (...)
        {
            // Fail-safe protection
        }

        // 4. Stereo Delay
        if (delayMix > 0.001f)
            processDelay(buffer);

        // 5. Stereo Reverb
        if (reverbMix > 0.001f)
            processReverb(buffer);

        // 6. Output Saturation & Gain
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float x = channelData[i] * outputGain;
                if (!std::isfinite(x)) x = 0.0f;
                // Soft analog saturation curve
                channelData[i] = std::tanh(x);
            }
        }
    }

private:
    double sampleRate = 44100.0;

    float transientAmount = 0.0f;
    float compAmount = 0.0f;
    float lowCutFreq = 80.0f;
    float bodyGainDb = 0.0f;
    float airGainDb = 3.0f;
    float outputGain = 1.0f;
    float mix = 1.0f;

    float reverbMix = 0.15f;
    float reverbSize = 0.5f;
    float delayMix = 0.0f;
    float delayTimeMs = 320.0f;
    float delayFeedback = 0.35f;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowCutFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> bodyFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> airFilter;

    juce::dsp::Reverb reverbProcessor;
    juce::AudioBuffer<float> reverbBuffer;

    std::vector<float> delayLineL;
    std::vector<float> delayLineR;
    int delayWriteIdx = 0;
    float delayDampedL = 0.0f;
    float delayDampedR = 0.0f;

    float envFastL = 0.0f;
    float envSlowL = 0.0f;
    float envFastR = 0.0f;
    float envSlowR = 0.0f;
    float transientActivity = 0.0f;

    float compEnvelope = 0.0f;
    float gainReductionDb = 0.0f;

    void updateFilters() noexcept
    {
        if (sampleRate <= 1000.0) return;

        try
        {
            *lowCutFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, lowCutFreq, 0.707f);

            *bodyFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, 250.0f, 0.9f, juce::Decibels::decibelsToGain(bodyGainDb));

            *airFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                sampleRate, 11500.0f, 0.707f, juce::Decibels::decibelsToGain(airGainDb));
        }
        catch (...)
        {
        }
    }

    void updateReverbParameters() noexcept
    {
        try
        {
            juce::dsp::Reverb::Parameters params;
            params.roomSize = juce::jlimit(0.1f, 0.95f, reverbSize);
            params.damping = 0.35f;
            params.wetLevel = 1.0f;
            params.dryLevel = 0.0f;
            params.width = 1.0f;
            params.freezeMode = 0.0f;
            reverbProcessor.setParameters(params);
        }
        catch (...)
        {
        }
    }

    void processTransients(juce::AudioBuffer<float>& buffer) noexcept
    {
        if (std::abs(transientAmount) < 0.1f) return;

        const int numSamples = buffer.getNumSamples();
        float* dataL = buffer.getWritePointer(0);
        float* dataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

        float alphaFastAtt = std::exp(-1.0f / (0.0012f * static_cast<float>(sampleRate)));
        float alphaFastRel = std::exp(-1.0f / (0.015f * static_cast<float>(sampleRate)));

        float alphaSlowAtt = std::exp(-1.0f / (0.025f * static_cast<float>(sampleRate)));
        float alphaSlowRel = std::exp(-1.0f / (0.070f * static_cast<float>(sampleRate)));

        float factor = (transientAmount / 100.0f) * 2.2f;
        float maxActivity = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            float absL = std::abs(dataL[i]);
            if (!std::isfinite(absL)) absL = 0.0f;

            envFastL = (absL > envFastL) ? (alphaFastAtt * envFastL + (1.0f - alphaFastAtt) * absL)
                                         : (alphaFastRel * envFastL + (1.0f - alphaFastRel) * absL);
            envSlowL = (absL > envSlowL) ? (alphaSlowAtt * envSlowL + (1.0f - alphaSlowAtt) * absL)
                                         : (alphaSlowRel * envSlowL + (1.0f - alphaSlowRel) * absL);

            float diffL = envFastL - envSlowL;
            if (diffL > 0.0f) maxActivity = std::max(maxActivity, diffL);

            float denomL = std::max(0.001f, envSlowL + 0.01f);
            float modGainL = 1.0f + factor * (diffL / denomL);
            modGainL = juce::jlimit(0.2f, 2.5f, modGainL);
            dataL[i] *= modGainL;

            if (dataR != nullptr)
            {
                float absR = std::abs(dataR[i]);
                if (!std::isfinite(absR)) absR = 0.0f;

                envFastR = (absR > envFastR) ? (alphaFastAtt * envFastR + (1.0f - alphaFastAtt) * absR)
                                             : (alphaFastRel * envFastR + (1.0f - alphaFastRel) * absR);
                envSlowR = (absR > envSlowR) ? (alphaSlowAtt * envSlowR + (1.0f - alphaSlowAtt) * absR)
                                             : (alphaSlowRel * envSlowR + (1.0f - alphaSlowRel) * absR);

                float diffR = envFastR - envSlowR;
                float denomR = std::max(0.001f, envSlowR + 0.01f);
                float modGainR = 1.0f + factor * (diffR / denomR);
                modGainR = juce::jlimit(0.2f, 2.5f, modGainR);
                dataR[i] *= modGainR;
            }
        }

        transientActivity = maxActivity;
    }

    void processCompression(juce::AudioBuffer<float>& buffer) noexcept
    {
        if (compAmount <= 0.001f)
        {
            gainReductionDb = 0.0f;
            return;
        }

        const int numSamples = buffer.getNumSamples();
        float* dataL = buffer.getWritePointer(0);
        float* dataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

        float thresholdDb = -32.0f * compAmount;
        float ratio = 4.0f + 4.0f * compAmount;
        float kneeDb = 6.0f;

        float alphaAtt = std::exp(-1.0f / (0.006f * static_cast<float>(sampleRate)));
        float alphaRel = std::exp(-1.0f / (0.110f * static_cast<float>(sampleRate)));

        float makeupDb = (-thresholdDb) * (1.0f - 1.0f / ratio) * 0.75f;
        float makeupGain = juce::Decibels::decibelsToGain(makeupDb);

        float currentGr = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            float peak = std::abs(dataL[i]);
            if (dataR != nullptr) peak = std::max(peak, std::abs(dataR[i]));
            if (!std::isfinite(peak)) peak = 0.0f;

            compEnvelope = (peak > compEnvelope) ? (alphaAtt * compEnvelope + (1.0f - alphaAtt) * peak)
                                                 : (alphaRel * compEnvelope + (1.0f - alphaRel) * peak);

            float envDb = juce::Decibels::gainToDecibels(compEnvelope + 1e-6f);
            float grDb = 0.0f;

            if (envDb <= thresholdDb - kneeDb * 0.5f)
            {
                grDb = 0.0f;
            }
            else if (envDb >= thresholdDb + kneeDb * 0.5f)
            {
                grDb = (thresholdDb + (envDb - thresholdDb) / ratio) - envDb;
            }
            else
            {
                float delta = envDb - thresholdDb + kneeDb * 0.5f;
                grDb = ((1.0f / ratio - 1.0f) * delta * delta) / (2.0f * kneeDb);
            }

            if (!std::isfinite(grDb)) grDb = 0.0f;
            currentGr = std::min(currentGr, grDb);

            float compGain = juce::Decibels::decibelsToGain(grDb) * makeupGain;
            if (!std::isfinite(compGain)) compGain = 1.0f;

            dataL[i] *= compGain;
            if (dataR != nullptr) dataR[i] *= compGain;
        }

        gainReductionDb = -currentGr;
    }

    void processDelay(juce::AudioBuffer<float>& buffer) noexcept
    {
        const int delayBufSize = static_cast<int>(delayLineL.size());
        if (delayBufSize <= 1) return;

        const int numSamples = buffer.getNumSamples();
        float* dataL = buffer.getWritePointer(0);
        float* dataR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

        int delayOffsetL = static_cast<int>((delayTimeMs * 0.001f) * static_cast<float>(sampleRate));
        int delayOffsetR = static_cast<int>((delayTimeMs * 0.001f * 1.05f) * static_cast<float>(sampleRate));

        delayOffsetL = juce::jlimit(1, delayBufSize - 1, delayOffsetL);
        delayOffsetR = juce::jlimit(1, delayBufSize - 1, delayOffsetR);

        float dampAlpha = 0.35f;

        for (int i = 0; i < numSamples; ++i)
        {
            int readIdxL = (delayWriteIdx - delayOffsetL + delayBufSize) % delayBufSize;
            int readIdxR = (delayWriteIdx - delayOffsetR + delayBufSize) % delayBufSize;

            float wetSampleL = delayLineL[readIdxL];
            float wetSampleR = delayLineR[readIdxR];

            if (!std::isfinite(wetSampleL)) wetSampleL = 0.0f;
            if (!std::isfinite(wetSampleR)) wetSampleR = 0.0f;

            delayDampedL = (1.0f - dampAlpha) * wetSampleL + dampAlpha * delayDampedL;
            delayDampedR = (1.0f - dampAlpha) * wetSampleR + dampAlpha * delayDampedR;

            float inL = std::isfinite(dataL[i]) ? dataL[i] : 0.0f;
            float inR = dataR ? (std::isfinite(dataR[i]) ? dataR[i] : 0.0f) : inL;
            float fbL = std::tanh(delayDampedL * delayFeedback);
            float fbR = std::tanh(delayDampedR * delayFeedback);
            delayLineL[delayWriteIdx] = std::tanh(inL + fbL);
            delayLineR[delayWriteIdx] = std::tanh(inR + fbR);

            dataL[i] += wetSampleL * delayMix;
            if (dataR != nullptr) dataR[i] += wetSampleR * delayMix;

            delayWriteIdx = (delayWriteIdx + 1) % delayBufSize;
        }
    }

    void processReverb(juce::AudioBuffer<float>& buffer) noexcept
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        if (numSamples <= 0)
            return;

        if (reverbBuffer.getNumSamples() < numSamples)
            reverbBuffer.setSize(2, numSamples * 2, false, false, true);

        try
        {
            // Copy audio to pre-allocated reverbBuffer
            for (int ch = 0; ch < 2; ++ch)
            {
                int srcCh = std::min(ch, numChannels - 1);
                reverbBuffer.copyFrom(ch, 0, buffer.getReadPointer(srcCh), numSamples);
            }

            juce::dsp::AudioBlock<float> revBlock(reverbBuffer);
            auto subBlock = revBlock.getSubBlock(0, static_cast<size_t>(numSamples));
            juce::dsp::ProcessContextReplacing<float> revContext(subBlock);
            reverbProcessor.process(revContext);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                float* dst = buffer.getWritePointer(ch);
                const float* rev = reverbBuffer.getReadPointer(std::min(ch, 1));
                for (int i = 0; i < numSamples; ++i)
                {
                    float r = rev[i];
                    if (std::isfinite(r))
                        dst[i] += r * reverbMix;
                }
            }
        }
        catch (...)
        {
        }
    }
};

} // namespace VeviAudio
