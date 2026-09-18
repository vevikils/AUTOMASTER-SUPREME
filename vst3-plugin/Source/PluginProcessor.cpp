#include "PluginProcessor.h"
#include "PluginEditor.h"

AutomasterSupremeAudioProcessor::AutomasterSupremeAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

AutomasterSupremeAudioProcessor::~AutomasterSupremeAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AutomasterSupremeAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Input & Low Cut
    params.push_back(std::make_unique<juce::AudioParameterFloat>("inGain", "Input Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lowCut", "Low Cut", juce::NormalisableRange<float>(20.0f, 80.0f, 1.0f), 30.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("lowCutBypass", "Low Cut Bypass", false));

    // 5-Band Parametric EQ
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqLowShelfGain", "EQ Sub Gain", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 1.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqLowMidGain", "EQ Low-Mid Gain", juce::NormalisableRange<float>(-9.0f, 9.0f, 0.1f), -1.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqMidGain", "EQ Mid Gain", juce::NormalisableRange<float>(-9.0f, 9.0f, 0.1f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqHighMidGain", "EQ Clarity Gain", juce::NormalisableRange<float>(-9.0f, 9.0f, 0.1f), 1.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqHighShelfGain", "EQ Air Gain", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 1.8f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("eqBypass", "EQ Bypass", false));

    // Tape & Tube Saturation
    params.push_back(std::make_unique<juce::AudioParameterFloat>("satDrive", "Saturation Drive", juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 12.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("satWarmth", "Saturation Warmth", juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 20.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("satMode", "Saturation Mode", juce::StringArray { "Analog Tape", "Tube Triode", "Class-A", "Clean" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>("satBypass", "Sat Bypass", false));

    // Multiband Dynamics
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mbLowThresh", "MB Low Thresh", juce::NormalisableRange<float>(-35.0f, -5.0f, 0.5f), -16.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mbMidThresh", "MB Mid Thresh", juce::NormalisableRange<float>(-35.0f, -5.0f, 0.5f), -14.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mbHighThresh", "MB High Thresh", juce::NormalisableRange<float>(-35.0f, -5.0f, 0.5f), -14.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("mbBypass", "MB Bypass", false));

    // Stereo Width & Mono Maker
    params.push_back(std::make_unique<juce::AudioParameterFloat>("stereoWidth", "Stereo Width", juce::NormalisableRange<float>(50.0f, 200.0f, 1.0f), 120.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("monoMakerFreq", "Mono Maker Freq", juce::NormalisableRange<float>(60.0f, 200.0f, 1.0f), 110.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("stereoBypass", "Stereo Bypass", false));

    // True-Peak Limiter & Loudness Maximizer (Spotify Reference)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("loudnessDrive", "Loudness Drive", juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ceiling", "True Peak Ceiling", juce::NormalisableRange<float>(-1.5f, 0.0f, 0.05f), AudioConstants::SPOTIFY_CEILING_DBTP));
    params.push_back(std::make_unique<juce::AudioParameterBool>("limiterBypass", "Limiter Bypass", false));

    // Master Out Gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>("outGain", "Output Gain", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));

    return { params.begin(), params.end() };
}

const juce::String AutomasterSupremeAudioProcessor::getName() const { return "AUTOMASTER SUPREME 3"; }
bool AutomasterSupremeAudioProcessor::acceptsMidi() const { return false; }
bool AutomasterSupremeAudioProcessor::producesMidi() const { return false; }
bool AutomasterSupremeAudioProcessor::isMidiEffect() const { return false; }
double AutomasterSupremeAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int AutomasterSupremeAudioProcessor::getNumPrograms() { return 1; }
int AutomasterSupremeAudioProcessor::getCurrentProgram() { return 0; }
void AutomasterSupremeAudioProcessor::setCurrentProgram(int) {}
const juce::String AutomasterSupremeAudioProcessor::getProgramName(int) { return {}; }
void AutomasterSupremeAudioProcessor::changeProgramName(int, const juce::String&) {}

void AutomasterSupremeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    dsp.prepare(sampleRate, samplesPerBlock);
}

void AutomasterSupremeAudioProcessor::releaseResources()
{
}

bool AutomasterSupremeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void AutomasterSupremeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    currentParams.inputGainDb = apvts.getRawParameterValue("inGain")->load();
    currentParams.lowCutFreq = apvts.getRawParameterValue("lowCut")->load();
    currentParams.lowCutBypass = apvts.getRawParameterValue("lowCutBypass")->load() > 0.5f;

    currentParams.eqLowShelfGain = apvts.getRawParameterValue("eqLowShelfGain")->load();
    currentParams.eqLowMidGain = apvts.getRawParameterValue("eqLowMidGain")->load();
    currentParams.eqMidGain = apvts.getRawParameterValue("eqMidGain")->load();
    currentParams.eqHighMidGain = apvts.getRawParameterValue("eqHighMidGain")->load();
    currentParams.eqHighShelfGain = apvts.getRawParameterValue("eqHighShelfGain")->load();
    currentParams.eqBypass = apvts.getRawParameterValue("eqBypass")->load() > 0.5f;

    currentParams.saturationDrive = apvts.getRawParameterValue("satDrive")->load();
    currentParams.saturationWarmth = apvts.getRawParameterValue("satWarmth")->load();
    currentParams.saturationMode = static_cast<int>(apvts.getRawParameterValue("satMode")->load());
    currentParams.saturationBypass = apvts.getRawParameterValue("satBypass")->load() > 0.5f;

    currentParams.mbLowThreshold = apvts.getRawParameterValue("mbLowThresh")->load();
    currentParams.mbMidThreshold = apvts.getRawParameterValue("mbMidThresh")->load();
    currentParams.mbHighThreshold = apvts.getRawParameterValue("mbHighThresh")->load();
    currentParams.mbBypass = apvts.getRawParameterValue("mbBypass")->load() > 0.5f;

    currentParams.stereoWidth = apvts.getRawParameterValue("stereoWidth")->load();
    currentParams.monoMakerFreq = apvts.getRawParameterValue("monoMakerFreq")->load();
    currentParams.stereoBypass = apvts.getRawParameterValue("stereoBypass")->load() > 0.5f;

    currentParams.loudnessDrive = apvts.getRawParameterValue("loudnessDrive")->load();
    currentParams.ceiling = apvts.getRawParameterValue("ceiling")->load();
    currentParams.limiterBypass = apvts.getRawParameterValue("limiterBypass")->load() > 0.5f;

    currentParams.outputGain = apvts.getRawParameterValue("outGain")->load();

    dsp.process(buffer, currentParams);
}

void AutomasterSupremeAudioProcessor::applyPreset(int index)
{
    // Spotify-calibrated presets
    switch (index)
    {
        case 0: // Spotify Reference
            apvts.getParameter("loudnessDrive")->setValueNotifyingHost(apvts.getParameterRange("loudnessDrive").convertTo0to1(2.0f));
            apvts.getParameter("ceiling")->setValueNotifyingHost(apvts.getParameterRange("ceiling").convertTo0to1(-1.0f));
            apvts.getParameter("eqLowShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowShelfGain").convertTo0to1(1.4f));
            apvts.getParameter("eqLowMidGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowMidGain").convertTo0to1(-1.2f));
            apvts.getParameter("eqHighShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqHighShelfGain").convertTo0to1(1.8f));
            apvts.getParameter("stereoWidth")->setValueNotifyingHost(apvts.getParameterRange("stereoWidth").convertTo0to1(120.0f));
            apvts.getParameter("satDrive")->setValueNotifyingHost(apvts.getParameterRange("satDrive").convertTo0to1(12.0f));
            break;
        case 1: // Modern Pop
            apvts.getParameter("loudnessDrive")->setValueNotifyingHost(apvts.getParameterRange("loudnessDrive").convertTo0to1(2.2f));
            apvts.getParameter("ceiling")->setValueNotifyingHost(apvts.getParameterRange("ceiling").convertTo0to1(-1.0f));
            apvts.getParameter("eqLowShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowShelfGain").convertTo0to1(1.5f));
            apvts.getParameter("eqLowMidGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowMidGain").convertTo0to1(-1.2f));
            apvts.getParameter("eqHighShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqHighShelfGain").convertTo0to1(1.6f));
            apvts.getParameter("stereoWidth")->setValueNotifyingHost(apvts.getParameterRange("stereoWidth").convertTo0to1(120.0f));
            apvts.getParameter("satDrive")->setValueNotifyingHost(apvts.getParameterRange("satDrive").convertTo0to1(14.0f));
            break;
        case 2: // Hip-Hop / Trap
            apvts.getParameter("loudnessDrive")->setValueNotifyingHost(apvts.getParameterRange("loudnessDrive").convertTo0to1(2.5f));
            apvts.getParameter("ceiling")->setValueNotifyingHost(apvts.getParameterRange("ceiling").convertTo0to1(-1.0f));
            apvts.getParameter("eqLowShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowShelfGain").convertTo0to1(2.4f));
            apvts.getParameter("eqLowMidGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowMidGain").convertTo0to1(-1.8f));
            apvts.getParameter("eqHighShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqHighShelfGain").convertTo0to1(1.8f));
            apvts.getParameter("stereoWidth")->setValueNotifyingHost(apvts.getParameterRange("stereoWidth").convertTo0to1(122.0f));
            apvts.getParameter("satDrive")->setValueNotifyingHost(apvts.getParameterRange("satDrive").convertTo0to1(18.0f));
            break;
        case 3: // EDM / Dance
            apvts.getParameter("loudnessDrive")->setValueNotifyingHost(apvts.getParameterRange("loudnessDrive").convertTo0to1(2.8f));
            apvts.getParameter("ceiling")->setValueNotifyingHost(apvts.getParameterRange("ceiling").convertTo0to1(-1.0f));
            apvts.getParameter("eqLowShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowShelfGain").convertTo0to1(2.2f));
            apvts.getParameter("eqLowMidGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowMidGain").convertTo0to1(-1.6f));
            apvts.getParameter("eqHighShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqHighShelfGain").convertTo0to1(2.0f));
            apvts.getParameter("stereoWidth")->setValueNotifyingHost(apvts.getParameterRange("stereoWidth").convertTo0to1(130.0f));
            apvts.getParameter("satDrive")->setValueNotifyingHost(apvts.getParameterRange("satDrive").convertTo0to1(18.0f));
            break;
        case 4: // Acoustic / Dynamic
            apvts.getParameter("loudnessDrive")->setValueNotifyingHost(apvts.getParameterRange("loudnessDrive").convertTo0to1(1.4f));
            apvts.getParameter("ceiling")->setValueNotifyingHost(apvts.getParameterRange("ceiling").convertTo0to1(-1.2f));
            apvts.getParameter("eqLowShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowShelfGain").convertTo0to1(0.6f));
            apvts.getParameter("eqLowMidGain")->setValueNotifyingHost(apvts.getParameterRange("eqLowMidGain").convertTo0to1(-0.8f));
            apvts.getParameter("eqHighShelfGain")->setValueNotifyingHost(apvts.getParameterRange("eqHighShelfGain").convertTo0to1(1.2f));
            apvts.getParameter("stereoWidth")->setValueNotifyingHost(apvts.getParameterRange("stereoWidth").convertTo0to1(110.0f));
            apvts.getParameter("satDrive")->setValueNotifyingHost(apvts.getParameterRange("satDrive").convertTo0to1(6.0f));
            break;
    }
}

bool AutomasterSupremeAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* AutomasterSupremeAudioProcessor::createEditor()
{
    return new AutomasterSupremeAudioProcessorEditor(*this);
}

void AutomasterSupremeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AutomasterSupremeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutomasterSupremeAudioProcessor();
}
