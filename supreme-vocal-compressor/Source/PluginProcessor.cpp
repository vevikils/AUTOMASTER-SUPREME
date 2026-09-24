#include "PluginProcessor.h"
#include "PluginEditor.h"

SupremeVocalBusCompressorAudioProcessor::SupremeVocalBusCompressorAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

SupremeVocalBusCompressorAudioProcessor::~SupremeVocalBusCompressorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SupremeVocalBusCompressorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // CLA-2A Standard Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "peak_reduction", "Peak Reduction",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 45.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "gain", "Gain (Output)",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 35.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "mode", "Mode",
        juce::StringArray { "COMPRESS", "LIMIT" }, 0));

    // Vintage High-Frequency Pre-Emphasis Trim (Hi-Freq flat vs vocal roll-off)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "hf_emphasis", "HF Sidechain Emphasis",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f), 0.0f));

    // Special Feature: Harmonic Sheen Exciter (Vocal High Sheen)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "sheen_amount", "Harmonic Sheen",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 30.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "sheen_freq", "Sheen Frequency",
        juce::NormalisableRange<float>(8000.0f, 16000.0f, 50.0f, 0.7f), 12000.0f));

    // Special Feature: 12AX7 Tube Harmonic Saturation
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tube_warmth", "Tube Warmth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 25.0f));

    // Mix (Dry / Wet)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "dry_wet", "Mix (Dry/Wet)",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));

    // Meter mode: GR, +4 IN, +10 OUT
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "meter_mode", "Meter Mode",
        juce::StringArray { "GR", "+4 IN", "+10 OUT" }, 0));

    return { params.begin(), params.end() };
}

const juce::String SupremeVocalBusCompressorAudioProcessor::getName() const
{
    return "Supreme Vocal Bus Compressor v1.3";
}

bool SupremeVocalBusCompressorAudioProcessor::acceptsMidi() const { return false; }
bool SupremeVocalBusCompressorAudioProcessor::producesMidi() const { return false; }
bool SupremeVocalBusCompressorAudioProcessor::isMidiEffect() const { return false; }
double SupremeVocalBusCompressorAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SupremeVocalBusCompressorAudioProcessor::getNumPrograms() { return 8; }
int SupremeVocalBusCompressorAudioProcessor::getCurrentProgram() { return 0; }
void SupremeVocalBusCompressorAudioProcessor::setCurrentProgram(int index) { loadPreset(index); }
const juce::String SupremeVocalBusCompressorAudioProcessor::getProgramName(int index)
{
    switch (index)
    {
        case 0: return "01. Travis Scott - Astroworld Vocal Glue";
        case 1: return "02. Drake - Silky OVO Bus Polish";
        case 2: return "03. T-Pain - Ultra Hard Auto-Tune Lock";
        case 3: return "04. Bad Bunny - Warm Latin Trap & Reggaeton";
        case 4: return "05. Rosalia - High Sheen Flamenco/Pop";
        case 5: return "06. Kendrick Lamar - Raw Punch & Dynamics";
        case 6: return "07. Billie Eilish - Whisper Intimacy & Air";
        case 7: return "08. The Weeknd - 80s Analog Retro Glow";
        default: return "Custom Preset";
    }
}
void SupremeVocalBusCompressorAudioProcessor::changeProgramName(int, const juce::String&) {}

void SupremeVocalBusCompressorAudioProcessor::loadPreset(int presetIndex)
{
    auto setP = [this](const juce::String& id, float val) {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(val));
    };

    switch (presetIndex)
    {
        case 0: // 01. Travis Scott - Astroworld Vocal Glue
            setP("peak_reduction", 64.0f);
            setP("gain", 44.0f);
            setP("mode", 1.0f); // Limit mode
            setP("hf_emphasis", 18.0f);
            setP("sheen_amount", 42.0f);
            setP("sheen_freq", 12500.0f);
            setP("tube_warmth", 48.0f);
            setP("dry_wet", 55.0f); // In-your-face parallel glue
            break;

        case 1: // 02. Drake - Silky OVO Bus Polish
            setP("peak_reduction", 38.0f);
            setP("gain", 35.0f);
            setP("mode", 0.0f); // Compress
            setP("hf_emphasis", 25.0f);
            setP("sheen_amount", 58.0f);
            setP("sheen_freq", 14200.0f);
            setP("tube_warmth", 20.0f);
            setP("dry_wet", 100.0f);
            break;

        case 2: // 03. T-Pain - Ultra Hard Auto-Tune Lock
            setP("peak_reduction", 76.0f);
            setP("gain", 52.0f);
            setP("mode", 1.0f); // Limit mode
            setP("hf_emphasis", -5.0f);
            setP("sheen_amount", 34.0f);
            setP("sheen_freq", 11500.0f);
            setP("tube_warmth", 35.0f);
            setP("dry_wet", 100.0f);
            break;

        case 3: // 04. Bad Bunny - Warm Latin Trap & Reggaeton
            setP("peak_reduction", 50.0f);
            setP("gain", 40.0f);
            setP("mode", 0.0f); // Compress
            setP("hf_emphasis", 10.0f);
            setP("sheen_amount", 26.0f);
            setP("sheen_freq", 10800.0f);
            setP("tube_warmth", 56.0f);
            setP("dry_wet", 90.0f);
            break;

        case 4: // 05. Rosalia - High Sheen Flamenco/Pop
            setP("peak_reduction", 34.0f);
            setP("gain", 33.0f);
            setP("mode", 0.0f); // Compress
            setP("hf_emphasis", 35.0f);
            setP("sheen_amount", 68.0f);
            setP("sheen_freq", 15000.0f);
            setP("tube_warmth", 16.0f);
            setP("dry_wet", 100.0f);
            break;

        case 5: // 06. Kendrick Lamar - Raw Punch & Dynamics
            // Moderate opto control with punchy mid presence and analog drive
            setP("peak_reduction", 54.0f);
            setP("gain", 42.0f);
            setP("mode", 0.0f); // Compress
            setP("hf_emphasis", -15.0f);
            setP("sheen_amount", 30.0f);
            setP("sheen_freq", 10000.0f);
            setP("tube_warmth", 50.0f);
            setP("dry_wet", 85.0f);
            break;

        case 6: // 07. Billie Eilish - Whisper Intimacy & Air
            // Ultra-delicate compression bringing out subtle breath nuances with high air sheen
            setP("peak_reduction", 32.0f);
            setP("gain", 36.0f);
            setP("mode", 0.0f); // Compress
            setP("hf_emphasis", 40.0f);
            setP("sheen_amount", 75.0f);
            setP("sheen_freq", 15500.0f);
            setP("tube_warmth", 10.0f);
            setP("dry_wet", 100.0f);
            break;

        case 7: // 08. The Weeknd - 80s Analog Retro Glow
            // Saturated vintage tube tape color with thick glue and shimmering high end
            setP("peak_reduction", 58.0f);
            setP("gain", 46.0f);
            setP("mode", 1.0f); // Limit
            setP("hf_emphasis", 12.0f);
            setP("sheen_amount", 50.0f);
            setP("sheen_freq", 13000.0f);
            setP("tube_warmth", 65.0f);
            setP("dry_wet", 70.0f); // Parallel vintage saturation
            break;
    }
}

void SupremeVocalBusCompressorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    dsp.prepare(sampleRate, samplesPerBlock);
}

void SupremeVocalBusCompressorAudioProcessor::releaseResources()
{
}

bool SupremeVocalBusCompressorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void SupremeVocalBusCompressorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    float peakReduction = apvts.getRawParameterValue("peak_reduction")->load();
    float gainMakeup    = apvts.getRawParameterValue("gain")->load();
    bool isLimiter      = (apvts.getRawParameterValue("mode")->load() > 0.5f);
    float sheenAmount   = apvts.getRawParameterValue("sheen_amount")->load();
    float sheenFreq     = apvts.getRawParameterValue("sheen_freq")->load();
    float tubeWarmth    = apvts.getRawParameterValue("tube_warmth")->load();
    float hfEmphasis    = apvts.getRawParameterValue("hf_emphasis")->load();
    float dryWet        = apvts.getRawParameterValue("dry_wet")->load();

    dsp.processBlock(buffer, peakReduction, gainMakeup, isLimiter,
                     sheenAmount, sheenFreq, tubeWarmth, hfEmphasis, dryWet);
}

bool SupremeVocalBusCompressorAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* SupremeVocalBusCompressorAudioProcessor::createEditor()
{
    return new SupremeVocalBusCompressorAudioProcessorEditor(*this);
}

void SupremeVocalBusCompressorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SupremeVocalBusCompressorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SupremeVocalBusCompressorAudioProcessor();
}
