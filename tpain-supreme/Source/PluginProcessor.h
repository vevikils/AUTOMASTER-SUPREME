#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "PitchEngine.h"
#include "VocalFXDSP.h"

class TPainSupremeAudioProcessor : public juce::AudioProcessor
{
public:
    TPainSupremeAudioProcessor();
    ~TPainSupremeAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    struct ArtistPreset
    {
        const char* name;
        float retuneSpeed;
        float transition;
        float variation;
        float centering;
        float transients;
        float compression;
        float lowCut;
        float body;
        float air;
        float reverbMix;
        float reverbSize;
        float delayMix;
        float delayTime;
        float delayFeedback;
        float output;
        float mix;
        float inputGain = 0.0f;
        float gateThresh = -50.0f;
        float gateAttack = 2.0f;
        float gateRelease = 200.0f;
        float stereoWidth = 100.0f;
    };

    static const std::vector<ArtistPreset>& getPresets();
    void loadPreset(int index);

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Telemetry for UI visualization
    juce::String getDetectedNoteName() const { return pitchEngine.getDetectedNoteName(); }
    juce::String getTargetNoteName() const { return pitchEngine.getTargetNoteName(); }
    float getCentsDeviation() const { return pitchEngine.getCentsDeviation(); }
    float getDetectedPitchHz() const { return pitchEngine.getDetectedPitchHz(); }
    float getTargetPitchHz() const { return pitchEngine.getTargetPitchHz(); }
    bool getIsVoiced() const { return pitchEngine.getIsVoiced(); }
    int getDetectedMidiNote() const { return pitchEngine.getDetectedMidiNote(); }
    int getTargetMidiNote() const { return pitchEngine.getTargetMidiNote(); }
    float getConfidence() const { return pitchEngine.getConfidence(); }
    float getGainReductionDb() const { return vocalFX.getGainReductionDb(); }
    float getTransientActivity() const { return vocalFX.getTransientActivity(); }
    float getGateGainReductionDb() const { return gateGainReductionDb.load(std::memory_order_relaxed); }
    bool getIsGateOpen() const { return gateIsOpen.load(std::memory_order_relaxed); }
    float getInputMeterPeakDb() const { return inputMeterPeakDb.load(std::memory_order_relaxed); }
    float getOutputMeterPeakDb() const { return outputMeterPeakDb.load(std::memory_order_relaxed); }
    float getMeasuredLatencyMs() const
    {
        double sr = getSampleRate();
        if (sr <= 0) sr = 44100.0;
        return static_cast<float>((getLatencySamples() * 1000.0) / sr);
    }

    int getLastHostBlockSize() const noexcept { return lastHostBlockSize.load(std::memory_order_relaxed); }
    int getSelectedBufferSizeIndex() const noexcept
    {
        return paramBufferSize ? static_cast<int>(paramBufferSize->load(std::memory_order_relaxed)) : 0;
    }
    int getEffectiveBufferSizeSamples() const noexcept
    {
        int idx = getSelectedBufferSizeIndex();
        if (idx <= 0) return std::max(64, lastHostBlockSize.load(std::memory_order_relaxed));
        return 64 << (idx - 1);
    }

    // Parameter ID constants
    static constexpr const char* ID_INPUT = "inputGain";
    static constexpr const char* ID_ROOT = "rootNote";
    static constexpr const char* ID_SCALE = "scaleType";
    static constexpr const char* ID_RETUNE_SPEED = "retuneSpeed";
    static constexpr const char* ID_VARIATION = "variation";
    static constexpr const char* ID_CENTERING = "centering";
    static constexpr const char* ID_TRANSITION = "transition";
    static constexpr const char* ID_TRANSIENTS = "transients";
    static constexpr const char* ID_COMPRESSION = "compression";
    static constexpr const char* ID_LOW_CUT = "lowCut";
    static constexpr const char* ID_BODY = "bodyEq";
    static constexpr const char* ID_AIR = "airEq";
    static constexpr const char* ID_REVERB_MIX = "reverbMix";
    static constexpr const char* ID_REVERB_SIZE = "reverbSize";
    static constexpr const char* ID_DELAY_MIX = "delayMix";
    static constexpr const char* ID_DELAY_TIME = "delayTime";
    static constexpr const char* ID_DELAY_FEEDBACK = "delayFeedback";
    static constexpr const char* ID_STEREO_WIDTH = "stereoWidth";
    static constexpr const char* ID_OUTPUT = "outputGain";
    static constexpr const char* ID_MIX = "mix";
    // Noise Gate
    static constexpr const char* ID_GATE_THRESH  = "gateThreshold";
    static constexpr const char* ID_GATE_ATTACK  = "gateAttack";
    static constexpr const char* ID_GATE_RELEASE = "gateRelease";
    // Buffer Size / DAW Sync
    static constexpr const char* ID_BUFFER_SIZE  = "bufferSize";

private:
    VeviAudio::PitchEngine pitchEngine;
    VeviAudio::VocalFXDSP vocalFX;

    // Parameter atomic pointers
    std::atomic<float>* paramInput = nullptr;
    std::atomic<float>* paramRoot = nullptr;
    std::atomic<float>* paramScale = nullptr;
    std::atomic<float>* paramRetuneSpeed = nullptr;
    std::atomic<float>* paramVariation = nullptr;
    std::atomic<float>* paramCentering = nullptr;
    std::atomic<float>* paramTransition = nullptr;
    std::atomic<float>* paramTransients = nullptr;
    std::atomic<float>* paramCompression = nullptr;
    std::atomic<float>* paramLowCut = nullptr;
    std::atomic<float>* paramBody = nullptr;
    std::atomic<float>* paramAir = nullptr;
    std::atomic<float>* paramReverbMix = nullptr;
    std::atomic<float>* paramReverbSize = nullptr;
    std::atomic<float>* paramDelayMix = nullptr;
    std::atomic<float>* paramDelayTime = nullptr;
    std::atomic<float>* paramDelayFeedback = nullptr;
    std::atomic<float>* paramStereoWidth = nullptr;
    std::atomic<float>* paramOutput = nullptr;
    std::atomic<float>* paramMix = nullptr;
    // Noise Gate atomic pointers
    std::atomic<float>* paramGateThresh  = nullptr;
    std::atomic<float>* paramGateAttack  = nullptr;
    std::atomic<float>* paramGateRelease = nullptr;
    // Buffer Size atomic pointer
    std::atomic<float>* paramBufferSize  = nullptr;
    std::atomic<int> lastHostBlockSize { 128 };
    // Gate DSP state (per-channel envelope follower)
    float gateEnv[2] = { 1.0f, 1.0f };
    std::atomic<float> gateGainReductionDb { 0.0f };
    std::atomic<bool> gateIsOpen { true };

    // Real-time Peak Meters for Gain Staging
    std::atomic<float> inputMeterPeakDb { -60.0f };
    std::atomic<float> outputMeterPeakDb { -60.0f };

    juce::AudioBuffer<float> dryBuffer;

    // DC Blocker & Anti-Pop Filter States
    float dcBlockerX1 = 0.0f;
    float dcBlockerY1 = 0.0f;
    float dcBlockerX2 = 0.0f;
    float dcBlockerY2 = 0.0f;

    int currentProgramIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TPainSupremeAudioProcessor)
};
