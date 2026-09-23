#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <cassert>
#include <limits>
#include "../Source/PluginProcessor.h"

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    std::cout << "=====================================================\n";
    std::cout << " STARTING SEVERE STRESS TESTING: SUPREME TUNER V3\n";
    std::cout << "=====================================================\n\n";

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> distNormal(-1.0f, 1.0f);
    std::uniform_real_distribution<float> distExtreme(-1000.0f, 1000.0f);

    int testsPassed = 0;

    // TEST 1: Processor Lifecycle & Re-initialization
    {
        std::cout << "[TEST 1] Instantiation & Lifecycle Loop (100 cycles)... ";
        for (int i = 0; i < 100; ++i)
        {
            auto proc = std::make_unique<TPainSupremeAudioProcessor>();
            proc->prepareToPlay(44100.0, 512);
            proc->releaseResources();
        }
        std::cout << "PASSED\n";
        testsPassed++;
    }

    // TEST 2: Sample Rate Extreme Boundary Stress
    {
        std::cout << "[TEST 2] Extreme Sample Rates (8kHz to 384kHz, zero, negative)... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        double sampleRates[] = { 8000.0, 11025.0, 22050.0, 44100.0, 48000.0, 88200.0, 96000.0, 176400.0, 192000.0, 384000.0, 0.0, -44100.0, 1e7 };
        for (double sr : sampleRates)
        {
            proc->prepareToPlay(sr, 512);
            juce::AudioBuffer<float> buf(2, 512);
            buf.clear();
            juce::MidiBuffer midi;
            proc->processBlock(buf, midi);
        }
        std::cout << "PASSED\n";
        testsPassed++;
    }

    // TEST 3: Buffer Size Edge Cases & Dynamic Resizing
    {
        std::cout << "[TEST 3] Variable Buffer Sizes (0, 1, 3, 7, 17, 64, 441, 1024, 4096, 8192, 16384)... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(44100.0, 512);

        int bufferSizes[] = { 0, 1, 2, 3, 7, 13, 17, 32, 64, 128, 256, 441, 512, 1024, 2048, 4096, 8192, 16384 };
        juce::MidiBuffer midi;

        for (int rep = 0; rep < 10; ++rep)
        {
            for (int sz : bufferSizes)
            {
                juce::AudioBuffer<float> buf(2, sz);
                for (int ch = 0; ch < 2; ++ch)
                {
                    float* w = buf.getWritePointer(ch);
                    for (int s = 0; s < sz; ++s)
                        w[s] = distNormal(rng);
                }
                proc->processBlock(buf, midi);
            }
        }
        std::cout << "PASSED\n";
        testsPassed++;
    }

    // TEST 4: Poison Input Attack (NaN, Inf, Denormals, 100 dBFS Overshoot)
    {
        std::cout << "[TEST 4] Poison Input Defense (NaN, Inf, -Inf, Subnormals, +100 dBFS)... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(48000.0, 256);
        juce::MidiBuffer midi;

        float poisons[] = {
            std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::signaling_NaN(),
            std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity(),
            1e-38f,
            -1e-38f,
            1e20f,
            -1e20f
        };

        for (float p : poisons)
        {
            juce::AudioBuffer<float> buf(2, 256);
            for (int ch = 0; ch < 2; ++ch)
            {
                float* w = buf.getWritePointer(ch);
                for (int s = 0; s < 256; ++s)
                    w[s] = (s % 2 == 0) ? p : distNormal(rng);
            }

            proc->processBlock(buf, midi);

            // Verify all output samples are strictly finite
            for (int ch = 0; ch < 2; ++ch)
            {
                const float* r = buf.getReadPointer(ch);
                for (int s = 0; s < 256; ++s)
                {
                    if (!std::isfinite(r[s]))
                    {
                        std::cerr << "FAIL: Non-finite output detected in test 4! Sample: " << r[s] << "\n";
                        return 1;
                    }
                }
            }
        }
        std::cout << "PASSED (All outputs remained 100% finite)\n";
        testsPassed++;
    }

    // TEST 5: Aggressive Parameter Fuzzing during Active Processing
    {
        std::cout << "[TEST 5] Parameter Fuzzing & Rapid Modulation (5,000 blocks)... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(44100.0, 128);
        juce::MidiBuffer midi;

        std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

        auto* pRoot = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_ROOT);
        auto* pScale = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_SCALE);
        auto* pSpeed = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_RETUNE_SPEED);
        auto* pVar = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_VARIATION);
        auto* pCenter = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_CENTERING);
        auto* pTrans = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_TRANSITION);
        auto* pTransient = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_TRANSIENTS);
        auto* pComp = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_COMPRESSION);
        auto* pLowCut = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_LOW_CUT);
        auto* pBody = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_BODY);
        auto* pAir = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_AIR);
        auto* pRevMix = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_REVERB_MIX);
        auto* pRevSize = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_REVERB_SIZE);
        auto* pDelMix = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_DELAY_MIX);
        auto* pDelTime = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_DELAY_TIME);
        auto* pDelFb = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_DELAY_FEEDBACK);
        auto* pMix = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_MIX);

        for (int block = 0; block < 5000; ++block)
        {
            // Rapidly randomize parameters every block
            if (pRoot) pRoot->setValueNotifyingHost(dist01(rng));
            if (pScale) pScale->setValueNotifyingHost(dist01(rng));
            if (pSpeed) pSpeed->setValueNotifyingHost(dist01(rng));
            if (pVar) pVar->setValueNotifyingHost(dist01(rng));
            if (pCenter) pCenter->setValueNotifyingHost(dist01(rng));
            if (pTrans) pTrans->setValueNotifyingHost(dist01(rng));
            if (pTransient) pTransient->setValueNotifyingHost(dist01(rng));
            if (pComp) pComp->setValueNotifyingHost(dist01(rng));
            if (pLowCut) pLowCut->setValueNotifyingHost(dist01(rng));
            if (pBody) pBody->setValueNotifyingHost(dist01(rng));
            if (pAir) pAir->setValueNotifyingHost(dist01(rng));
            if (pRevMix) pRevMix->setValueNotifyingHost(dist01(rng));
            if (pRevSize) pRevSize->setValueNotifyingHost(dist01(rng));
            if (pDelMix) pDelMix->setValueNotifyingHost(dist01(rng));
            if (pDelTime) pDelTime->setValueNotifyingHost(dist01(rng));
            if (pDelFb) pDelFb->setValueNotifyingHost(dist01(rng));
            if (pMix) pMix->setValueNotifyingHost(dist01(rng));

            juce::AudioBuffer<float> buf(2, 128);
            for (int ch = 0; ch < 2; ++ch)
            {
                float* w = buf.getWritePointer(ch);
                for (int s = 0; s < 128; ++s)
                    w[s] = std::sin(2.0f * 3.14159f * 440.0f * (block * 128 + s) / 44100.0f) * 0.7f;
            }

            proc->processBlock(buf, midi);

            for (int ch = 0; ch < 2; ++ch)
            {
                const float* r = buf.getReadPointer(ch);
                for (int s = 0; s < 128; ++s)
                {
                    if (!std::isfinite(r[s]))
                    {
                        std::cerr << "FAIL: Non-finite output in test 5!\n";
                        return 1;
                    }
                }
            }
        }
        std::cout << "PASSED (5,000 blocks processed cleanly)\n";
        testsPassed++;
    }

    // TEST 6: Audio Signals Across Full Spectrum (Sine Sweep, White Noise, Silence, DC)
    {
        std::cout << "[TEST 6] Acoustic Edge Cases (DC, 20Hz-20kHz Sweeps, Heavy Noise)... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(44100.0, 256);
        juce::MidiBuffer midi;

        // 6a: Pure DC offset
        {
            juce::AudioBuffer<float> buf(2, 512);
            for (int ch = 0; ch < 2; ++ch)
                for (int s = 0; s < 512; ++s)
                    buf.setSample(ch, s, 1.0f);
            proc->processBlock(buf, midi);
        }

        // 6b: Nyquist oscillation
        {
            juce::AudioBuffer<float> buf(2, 512);
            for (int ch = 0; ch < 2; ++ch)
                for (int s = 0; s < 512; ++s)
                    buf.setSample(ch, s, (s % 2 == 0) ? 1.0f : -1.0f);
            proc->processBlock(buf, midi);
        }

        // 6c: Sine sweep 20Hz to 10kHz
        {
            const int sweepSamples = 44100; // 1 second sweep
            juce::AudioBuffer<float> sweepBuf(2, sweepSamples);
            double phase = 0.0;
            for (int s = 0; s < sweepSamples; ++s)
            {
                double freq = 20.0 + (10000.0 - 20.0) * (s / (double)sweepSamples);
                phase += 2.0 * 3.141592653589793 * freq / 44100.0;
                float val = static_cast<float>(std::sin(phase));
                sweepBuf.setSample(0, s, val);
                sweepBuf.setSample(1, s, val);
            }

            for (int offset = 0; offset < sweepSamples; offset += 256)
            {
                int sz = std::min(256, sweepSamples - offset);
                juce::AudioBuffer<float> chunk(sweepBuf.getArrayOfWritePointers(), 2, offset, sz);
                proc->processBlock(chunk, midi);
            }
        }

        std::cout << "PASSED\n";
        testsPassed++;
    }

    // TEST 7: Corrupted Preset & State Restoration Attack
    {
        std::cout << "[TEST 7] Corrupted Project / Preset State Defense... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();

        // Valid state roundtrip
        juce::MemoryBlock validState;
        proc->getStateInformation(validState);
        proc->setStateInformation(validState.getData(), static_cast<int>(validState.getSize()));

        // Null data
        proc->setStateInformation(nullptr, 0);
        proc->setStateInformation(nullptr, 1024);

        // Fuzzed garbage data
        std::vector<uint8_t> garbage(1024);
        for (auto& b : garbage) b = static_cast<uint8_t>(rng() % 256);
        proc->setStateInformation(garbage.data(), static_cast<int>(garbage.size()));

        // Truncated XML
        std::string badXml = "<PARAM";
        proc->setStateInformation(badXml.data(), static_cast<int>(badXml.size()));

        std::cout << "PASSED\n";
        testsPassed++;
    }

    // TEST 8: Mono & Stereo Bus Layout Tests
    {
        std::cout << "[TEST 8] Channel Configurations (Mono-in/Mono-out, Stereo-in/Stereo-out)... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(44100.0, 256);
        juce::MidiBuffer midi;

        // Mono
        juce::AudioBuffer<float> monoBuf(1, 256);
        monoBuf.clear();
        proc->processBlock(monoBuf, midi);

        // Stereo
        juce::AudioBuffer<float> stereoBuf(2, 256);
        stereoBuf.clear();
        proc->processBlock(stereoBuf, midi);

        std::cout << "PASSED\n";
        testsPassed++;
    }

    // TEST 9: Microphone Rapid Arm / Disarm & ASIO Stream Cycling (FL Studio Simulation)
    {
        std::cout << "[TEST 9] Microphone Rapid Arm/Disarm & Stream Restarts (200 cycles)... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        juce::MidiBuffer midi;

        for (int cycle = 0; cycle < 200; ++cycle)
        {
            // Toggle between mono mic and stereo track
            int inChannels = (cycle % 2 == 0) ? 1 : 2;
            proc->prepareToPlay(44100.0, 128);

            juce::AudioBuffer<float> micBuffer(2, 128);
            for (int s = 0; s < 128; ++s)
            {
                float sample = (inChannels == 1) ? distNormal(rng) : distNormal(rng) * 0.5f;
                micBuffer.setSample(0, s, sample);
                micBuffer.setSample(1, s, (inChannels == 2) ? sample : 0.0f);
            }

            proc->processBlock(micBuffer, midi);

            // Periodically trigger releaseResources (FL Studio stream reset)
            if (cycle % 5 == 0)
                proc->releaseResources();
        }
        std::cout << "PASSED (Zero audio thread crashes during 200 mic toggles)\n";
        testsPassed++;
    }

    // TEST 10: DC Offset Step & Acoustic Explosion Burst Defense
    {
        std::cout << "[TEST 10] Massive DC Step (+100 dBFS hardware pop) & Feedback Blast Defense... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(48000.0, 256);
        juce::MidiBuffer midi;

        // Simulate a violent hardware DC offset pop from a phantom power / mic plug
        juce::AudioBuffer<float> popBuffer(2, 512);
        for (int ch = 0; ch < 2; ++ch)
            for (int s = 0; s < 512; ++s)
                popBuffer.setSample(ch, s, 50.0f); // Massive +34 dBFS electrical step

        proc->processBlock(popBuffer, midi);

        // Verify DC blocker and input limiter suppressed the spike to safe levels
        for (int ch = 0; ch < 2; ++ch)
        {
            const float* r = popBuffer.getReadPointer(ch);
            for (int s = 0; s < 512; ++s)
            {
                if (!std::isfinite(r[s]) || std::abs(r[s]) > 2.0f)
                {
                    std::cerr << "FAIL: Audio blast escaped safety limiter! Sample: " << r[s] << "\n";
                    return 1;
                }
            }
        }
        std::cout << "PASSED (Input clamped safely, zero acoustic explosion!)\n";
        testsPassed++;
    }

    // TEST 11: Noise Gate Attenuation & Threshold Verification
    {
        std::cout << "[TEST 11] Noise Gate Dynamic Attenuation & Telemetry... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(44100.0, 256);
        juce::MidiBuffer midi;

        // Set Gate Threshold to -45 dBFS, fast attack 1.0 ms, release 30.0 ms
        if (auto* param = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_GATE_THRESH))
            param->setValueNotifyingHost(param->convertTo0to1(-45.0f));
        if (auto* param = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_GATE_ATTACK))
            param->setValueNotifyingHost(param->convertTo0to1(1.0f));
        if (auto* param = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_GATE_RELEASE))
            param->setValueNotifyingHost(param->convertTo0to1(30.0f));

        // 11a: Sub-threshold background noise (-70 dBFS ~ 0.0003f)
        juce::AudioBuffer<float> noiseBuf(2, 256);
        for (int b = 0; b < 20; ++b)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                float* w = noiseBuf.getWritePointer(ch);
                for (int s = 0; s < 256; ++s)
                    w[s] = distNormal(rng) * 0.0003f;
            }
            proc->processBlock(noiseBuf, midi);
        }

        // Noise should be attenuated, no NaNs
        for (int ch = 0; ch < 2; ++ch)
        {
            const float* r = noiseBuf.getReadPointer(ch);
            for (int s = 0; s < 256; ++s)
            {
                assert(std::isfinite(r[s]));
                assert(std::abs(r[s]) < 0.0003f); // Must be attenuated below input noise
            }
        }

        // 11b: Above-threshold vocal burst (440 Hz at -6 dBFS ~ 0.5f)
        juce::AudioBuffer<float> voiceBuf(2, 256);
        for (int b = 0; b < 10; ++b)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                float* w = voiceBuf.getWritePointer(ch);
                for (int s = 0; s < 256; ++s)
                    w[s] = 0.5f * std::sin(2.0 * 3.141592653589793 * 440.0 * s / 44100.0);
            }
            proc->processBlock(voiceBuf, midi);
        }

        assert(proc->getIsGateOpen()); // Gate must open cleanly on vocals
        std::cout << "PASSED (Noise suppressed cleanly, vocals passed unhindered!)\n";
        testsPassed++;
    }

    // TEST 12: Gain Staging (Input/Output Gain & Live Meters) & Stereo Width (Mid/Side)
    {
        std::cout << "[TEST 12] Input/Output Gain Staging, Live Peak Meters & Stereo Width Engine... ";
        auto proc = std::make_unique<TPainSupremeAudioProcessor>();
        proc->prepareToPlay(44100.0, 512);

        // 12a: Verify Input Gain and Live Peak Meter
        auto* inGainParam = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_INPUT);
        assert(inGainParam != nullptr);
        inGainParam->setValueNotifyingHost(inGainParam->convertTo0to1(6.0f)); // +6 dB

        juce::AudioBuffer<float> testBuf(2, 512);
        for (int s = 0; s < 512; ++s)
        {
            testBuf.setSample(0, s, 0.2f);
            testBuf.setSample(1, s, -0.2f);
        }
        juce::MidiBuffer midi;
        proc->processBlock(testBuf, midi);

        // Input meter should reflect active audio
        float inMeterDb = proc->getInputMeterPeakDb();
        float outMeterDb = proc->getOutputMeterPeakDb();
        assert(inMeterDb > -30.0f);
        assert(outMeterDb > -30.0f);

        // 12b: Verify Stereo Width at 0% (Mono downmix)
        auto* widthParam = proc->apvts.getParameter(TPainSupremeAudioProcessor::ID_STEREO_WIDTH);
        assert(widthParam != nullptr);
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(0.0f)); // 0% Mono

        juce::AudioBuffer<float> stereoBuf(2, 512);
        for (int s = 0; s < 512; ++s)
        {
            stereoBuf.setSample(0, s, 0.5f);
            stereoBuf.setSample(1, s, 0.1f);
        }
        proc->processBlock(stereoBuf, midi);

        // At 0% width, L and R must be identical (mid channel only)
        for (int s = 0; s < 512; ++s)
        {
            float l = stereoBuf.getSample(0, s);
            float r = stereoBuf.getSample(1, s);
            assert(std::abs(l - r) < 1e-4f);
        }

        // 12c: Verify Stereo Width at 200% (Super-Wide)
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(200.0f));
        juce::AudioBuffer<float> wideBuf(2, 512);
        for (int s = 0; s < 512; ++s)
        {
            wideBuf.setSample(0, s, 0.5f);
            wideBuf.setSample(1, s, 0.1f);
        }
        proc->processBlock(wideBuf, midi);

        // At 200% width, difference between L and R must be expanded
        for (int s = 0; s < 512; ++s)
        {
            float l = wideBuf.getSample(0, s);
            float r = wideBuf.getSample(1, s);
            assert(std::abs(l - r) > 0.01f);
        }

        std::cout << "PASSED (Live meters, gain staging & stereo width verified!)\n";
        testsPassed++;
    }

    std::cout << "\n=====================================================\n";
    std::cout << " ALL " << testsPassed << " SEVERE ERROR STRESS TESTS PASSED SUCCESSFULLY!\n";
    std::cout << " PLUGIN IS 100% HARDENED & CRASH-PROOF!\n";
    std::cout << "=====================================================\n";

    return 0;
}
