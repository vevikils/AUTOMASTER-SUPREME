#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "VeviDSP.h"

class VeviProQAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int NUM_BANDS = VeviDSP::DynamicEQEngine::NUM_BANDS;

    VeviProQAudioProcessor();
    ~VeviProQAudioProcessor() override;

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

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // FFT visualizer interface for real-time PRE-EQ and POST-EQ spectrum & RTA Analyzer
    static constexpr int FFT_ORDER = 11;
    static constexpr int FFT_SIZE = 1 << FFT_ORDER;
    void pushPreSampleIntoFifo(float sample) noexcept;
    void pushPostSampleIntoFifo(float sample) noexcept;
    juce::dsp::FFT& getForwardFFT() { return forwardFFT; }
    juce::dsp::WindowingFunction<float>& getWindow() { return window; }
    bool getNextAudioBlockReady() { return nextFFTBlockReady.exchange(false); }
    const std::array<float, FFT_SIZE * 2>& getPreFFTData() const { return preFftData; }
    const std::array<float, FFT_SIZE * 2>& getPostFFTData() const { return postFftData; }

    // DSP Accessors
    double getMagnitudeForFrequency(double freq) const;
    double getBandMagnitudeForFrequency(int bandIdx, double freq) const;
    double getBandMagnitudeWithGain(int bandIdx, double freq, float gainDB) const;
    float getBandDynamicGain(int bandIdx) const { return dspEngine.getBandDynamicGain(bandIdx); }
    VeviDSP::BandSettings getBandSettings(int bandIdx) const;

    // Artist Presets Management
    struct BandPresetConfig
    {
        bool enabled = true;
        int shape = 0; // 0: Bell, 1: LowShelf, 2: HighShelf, 3: LowCut, 4: HighCut, 5: Notch, 6: BandPass, 7: Tilt
        float freq = 1000.0f;
        float gain = 0.0f;
        float q = 1.0f;
        bool dynEnabled = false;
        float dynRange = 0.0f;
        float dynThresh = -20.0f;
        int slope = 1; // 0: 6dB, 1: 12dB, 2: 18dB, 3: 24dB, 4: 48dB
        int routing = 0; // 0: Stereo, 1: Mid, 2: Side, 3: Left, 4: Right
    };

    struct ArtistPreset
    {
        juce::String name;
        juce::String category;
        std::array<BandPresetConfig, NUM_BANDS> bands;
        float masterGain = 0.0f;
    };

    static const std::vector<ArtistPreset>& getArtistPresets();
    void loadPreset(int presetIndex);
    int getCurrentPresetIndex() const { return currentPresetIndex; }

    // Fast atomic parameter caches
    struct BandParamAtomics
    {
        std::atomic<float>* enabled = nullptr;
        std::atomic<float>* type = nullptr;
        std::atomic<float>* freq = nullptr;
        std::atomic<float>* gain = nullptr;
        std::atomic<float>* q = nullptr;
        std::atomic<float>* dynEnabled = nullptr;
        std::atomic<float>* dynRange = nullptr;
        std::atomic<float>* threshold = nullptr;
        std::atomic<float>* slope = nullptr;
        std::atomic<float>* stereoRouting = nullptr;
    };

    static juce::String getParamId(int bandIdx, const juce::String& name)
    {
        return "band" + juce::String(bandIdx + 1) + "_" + name;
    }

private:
    VeviDSP::DynamicEQEngine dspEngine;
    std::array<BandParamAtomics, NUM_BANDS> bandParams;

    std::atomic<float>* paramMasterGain = nullptr;
    std::atomic<float>* paramGlobalBypass = nullptr;
    std::atomic<float>* paramPhaseMode = nullptr;

    // Pre-EQ and Post-EQ Spectrum Analyzer Data
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, FFT_SIZE> preFifo;
    std::array<float, FFT_SIZE * 2> preFftData;
    int preFifoIndex = 0;

    std::array<float, FFT_SIZE> postFifo;
    std::array<float, FFT_SIZE * 2> postFftData;
    int postFifoIndex = 0;

    std::atomic<bool> nextFFTBlockReady { false };
    int currentPresetIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VeviProQAudioProcessor)
};
