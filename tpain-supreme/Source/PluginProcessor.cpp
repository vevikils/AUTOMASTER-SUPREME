#include "PluginProcessor.h"
#include "PluginEditor.h"

TPainSupremeAudioProcessor::TPainSupremeAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    paramRoot = apvts.getRawParameterValue(ID_ROOT);
    paramScale = apvts.getRawParameterValue(ID_SCALE);
    paramRetuneSpeed = apvts.getRawParameterValue(ID_RETUNE_SPEED);
    paramVariation = apvts.getRawParameterValue(ID_VARIATION);
    paramCentering = apvts.getRawParameterValue(ID_CENTERING);
    paramTransition = apvts.getRawParameterValue(ID_TRANSITION);
    paramTransients = apvts.getRawParameterValue(ID_TRANSIENTS);
    paramCompression = apvts.getRawParameterValue(ID_COMPRESSION);
    paramLowCut = apvts.getRawParameterValue(ID_LOW_CUT);
    paramBody = apvts.getRawParameterValue(ID_BODY);
    paramAir = apvts.getRawParameterValue(ID_AIR);
    paramReverbMix = apvts.getRawParameterValue(ID_REVERB_MIX);
    paramReverbSize = apvts.getRawParameterValue(ID_REVERB_SIZE);
    paramDelayMix = apvts.getRawParameterValue(ID_DELAY_MIX);
    paramDelayTime = apvts.getRawParameterValue(ID_DELAY_TIME);
    paramDelayFeedback = apvts.getRawParameterValue(ID_DELAY_FEEDBACK);
    paramInput = apvts.getRawParameterValue(ID_INPUT);
    paramStereoWidth = apvts.getRawParameterValue(ID_STEREO_WIDTH);
    paramOutput = apvts.getRawParameterValue(ID_OUTPUT);
    paramMix    = apvts.getRawParameterValue(ID_MIX);
    paramGateThresh  = apvts.getRawParameterValue(ID_GATE_THRESH);
    paramGateAttack  = apvts.getRawParameterValue(ID_GATE_ATTACK);
    paramGateRelease = apvts.getRawParameterValue(ID_GATE_RELEASE);
    gateEnv[0] = 1.0f;
    gateEnv[1] = 1.0f;
}

TPainSupremeAudioProcessor::~TPainSupremeAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout TPainSupremeAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    juce::StringArray rootChoices = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    juce::StringArray scaleChoices = {
        "Major",
        "Minor (Natural)",
        "Harmonic Minor",
        "Melodic Minor",
        "Pentatonic Major",
        "Pentatonic Minor",
        "Blues",
        "Chromatic"
    };

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ID_ROOT, 1), "Root Note", rootChoices, 0)); // C

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(ID_SCALE, 1), "Scale", scaleChoices, 0)); // Major

    // Retune Speed: 0ms (T-Pain instant) to 400ms (Natural/Slow)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_RETUNE_SPEED, 1), "Retune Speed",
        juce::NormalisableRange<float>(0.0f, 400.0f, 0.1f, 0.35f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    // Pitch Variation (Humanize / Vibrato allowance): 0% (pure robot) to 100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_VARIATION, 1), "Pitch Variation",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Centering: Strength of locking to exact concert frequency (0% to 100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_CENTERING, 1), "Note Centering",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f, 1.0f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Note Transition Glide: 0ms (instant jump) to 250ms (smooth portamento)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_TRANSITION, 1), "Note Transition",
        juce::NormalisableRange<float>(0.0f, 250.0f, 0.5f, 0.4f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    // Transients: -100% (Soft/Smooth) to +100% (Punch/Crisp Attack)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_TRANSIENTS, 1), "Transients",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Compression: 0% to 100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_COMPRESSION, 1), "Compression",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f, 1.0f), 30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Light EQ Low Cut: 20 Hz to 200 Hz
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_LOW_CUT, 1), "Low Cut",
        juce::NormalisableRange<float>(20.0f, 200.0f, 1.0f, 0.5f), 80.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // Light EQ Body: -6 dB to +6 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_BODY, 1), "Body Warmth",
        juce::NormalisableRange<float>(-6.0f, 6.0f, 0.1f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Light EQ Air: 0 dB to 10 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_AIR, 1), "Vocal Air",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), 3.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Space FX: Reverb Mix (0% to 100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_REVERB_MIX, 1), "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f, 1.0f), 15.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Space FX: Reverb Size (0% to 100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_REVERB_SIZE, 1), "Reverb Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Space FX: Delay Mix (0% to 100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_DELAY_MIX, 1), "Delay Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Space FX: Delay Time (20 ms to 1000 ms)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_DELAY_TIME, 1), "Delay Time",
        juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f, 0.4f), 320.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    // Space FX: Delay Feedback (0% to 85%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_DELAY_FEEDBACK, 1), "Delay Feedback",
        juce::NormalisableRange<float>(0.0f, 85.0f, 0.5f, 1.0f), 35.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Input Gain: -24 dB to +24 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_INPUT, 1), "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Output Gain: -24 dB to +6 dB (pro mastering console range)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_OUTPUT, 1), "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f, 1.0f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Stereo Width: 0% (Mono) to 200% (Super-Wide), default 100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_STEREO_WIDTH, 1), "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f, 1.0f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Mix Dry/Wet: 0% to 100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_MIX, 1), "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f, 1.0f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // === NOISE GATE ===
    // Threshold: -80 dB to 0 dB (default -50 dB — safe floor)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_GATE_THRESH, 1), "Gate Threshold",
        juce::NormalisableRange<float>(-80.0f, 0.0f, 0.5f, 2.5f), -50.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Attack: 0.1 ms to 50 ms (how fast the gate opens)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_GATE_ATTACK, 1), "Gate Attack",
        juce::NormalisableRange<float>(0.1f, 50.0f, 0.1f, 0.5f), 2.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    // Release: 10 ms to 2000 ms (how fast the gate closes)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID(ID_GATE_RELEASE, 1), "Gate Release",
        juce::NormalisableRange<float>(10.0f, 2000.0f, 1.0f, 0.4f), 200.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    return { params.begin(), params.end() };
}

const juce::String TPainSupremeAudioProcessor::getName() const
{
    return "Supreme Tuner Real Time v3.5";
}

bool TPainSupremeAudioProcessor::acceptsMidi() const { return false; }
bool TPainSupremeAudioProcessor::producesMidi() const { return false; }
bool TPainSupremeAudioProcessor::isMidiEffect() const { return false; }
double TPainSupremeAudioProcessor::getTailLengthSeconds() const { return 0.0; }

const std::vector<TPainSupremeAudioProcessor::ArtistPreset>& TPainSupremeAudioProcessor::getPresets()
{
    // Columnas: name, retuneSpeed, transition, variation, centering, transients, compression, lowCut, body, air, reverbMix, reverbSize, delayMix, delayTime, delayFeedback, output, mix, inputGain, gateThresh, gateAttack, gateRelease, stereoWidth
    static const std::vector<ArtistPreset> presets = {
        // === ICONIC AUTO-TUNE CLASSICS ===
        { "T-Pain - Buy U a Drank",              0.0f,  0.0f,  0.0f, 100.0f, 35.0f, 65.0f,  95.0f,  2.0f, 4.5f, 15.0f, 45.0f, 10.0f, 250.0f, 25.0f, 0.0f, 100.0f, 0.0f, -50.0f, 1.5f, 180.0f, 100.0f },
        { "Travis Scott - Astroworld Sauce",     0.5f,  5.0f, 15.0f,  90.0f, 50.0f, 75.0f, 110.0f, -1.0f, 7.0f, 30.0f, 85.0f, 24.0f, 375.0f, 45.0f, 0.0f, 100.0f, 1.0f, -48.0f, 2.0f, 220.0f, 125.0f },
        { "Post Malone - Sunflower Smooth",     18.0f, 35.0f, 55.0f,  75.0f, 20.0f, 45.0f,  80.0f,  1.5f, 3.5f, 19.0f, 60.0f, 13.0f, 320.0f, 30.0f, 0.0f, 100.0f, 0.0f, -52.0f, 3.0f, 250.0f, 110.0f },
        { "The Weeknd - After Hours Shimmer",   12.0f, 20.0f, 40.0f,  85.0f, 25.0f, 60.0f,  85.0f,  0.5f, 6.0f, 34.0f, 80.0f, 17.0f, 420.0f, 35.0f, 0.0f, 100.0f, 0.5f, -50.0f, 2.5f, 300.0f, 130.0f },
        { "Bad Bunny - Conejo Trap Moderno",     1.0f,  8.0f, 20.0f,  95.0f, 40.0f, 70.0f, 100.0f,  2.5f, 4.0f, 17.0f, 50.0f, 15.0f, 280.0f, 35.0f, 0.0f, 100.0f, 1.5f, -46.0f, 1.8f, 160.0f, 105.0f },
        { "Rauw Alejandro - Afro & Dancehall",   5.0f, 12.0f, 30.0f,  90.0f, 30.0f, 55.0f,  90.0f,  1.0f, 5.5f, 24.0f, 65.0f, 19.0f, 340.0f, 38.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.0f, 200.0f, 120.0f },
        { "Billie Eilish - Intimate & Airy",    35.0f, 60.0f, 70.0f,  60.0f,-20.0f, 80.0f,  75.0f,  3.0f, 8.5f, 38.0f, 90.0f, 21.0f, 500.0f, 40.0f, 0.0f, 100.0f,-2.0f, -55.0f, 5.0f, 350.0f, 115.0f },
        { "Daft Punk - One More Time Vocoder",   0.0f,  0.0f,  0.0f, 100.0f, 45.0f, 85.0f, 120.0f,  3.5f, 5.0f, 21.0f, 55.0f, 26.0f, 180.0f, 50.0f, 0.0f, 100.0f, 2.0f, -44.0f, 1.0f, 140.0f, 100.0f },
        { "Rosalia - Motomami Flamenco Urban",   8.0f, 15.0f, 45.0f,  85.0f, 35.0f, 50.0f,  85.0f,  1.8f, 5.0f, 20.0f, 55.0f, 14.0f, 300.0f, 28.0f, 0.0f, 100.0f, 0.5f, -48.0f, 2.0f, 190.0f, 110.0f },
        { "Kanye West - 808s Heartbreak",        0.0f,  2.0f,  5.0f, 100.0f, 30.0f, 70.0f, 100.0f,  1.0f, 4.0f, 25.0f, 70.0f, 30.0f, 330.0f, 45.0f, 0.0f, 100.0f, 0.0f, -48.0f, 1.5f, 200.0f, 100.0f },
        { "Future - Pluto Auto-Tune Drip",       0.0f,  0.0f, 10.0f, 100.0f, 45.0f, 75.0f, 105.0f,  2.0f, 6.5f, 27.0f, 65.0f, 21.0f, 260.0f, 40.0f, 0.0f, 100.0f, 1.0f, -46.0f, 1.2f, 180.0f, 115.0f },
        { "Drake - Certified OVO Flow",         14.0f, 25.0f, 50.0f,  80.0f, 20.0f, 50.0f,  80.0f,  1.5f, 3.0f, 17.0f, 50.0f, 13.0f, 320.0f, 25.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.5f, 220.0f, 105.0f },
        { "Cher - Believe 90s Classic",          0.0f,  0.0f,  0.0f, 100.0f, 25.0f, 60.0f,  90.0f,  0.0f, 3.5f, 21.0f, 60.0f,  8.0f, 200.0f, 20.0f, 0.0f, 100.0f, 0.0f, -52.0f, 1.0f, 150.0f, 100.0f },
        { "Duki - Trap Argentino Fuego",         0.0f,  4.0f, 15.0f,  95.0f, 40.0f, 70.0f,  95.0f,  2.2f, 5.5f, 19.0f, 55.0f, 17.0f, 290.0f, 35.0f, 0.0f, 100.0f, 1.5f, -45.0f, 1.5f, 170.0f, 110.0f },
        { "Ariana Grande - Modern Bright Lead", 22.0f, 40.0f, 65.0f,  70.0f, 15.0f, 40.0f,  80.0f,  0.5f, 7.5f, 30.0f, 75.0f, 15.0f, 360.0f, 30.0f, 0.0f, 100.0f, 0.0f, -52.0f, 3.0f, 240.0f, 125.0f },
        // === LATAM & ESPANA ===
        { "J Balvin - Colores Reggaeton",        2.0f,  5.0f, 10.0f,  95.0f, 45.0f, 72.0f, 100.0f,  3.0f, 4.0f, 15.0f, 48.0f,  8.0f, 240.0f, 30.0f, 0.0f, 100.0f, 1.0f, -48.0f, 2.0f, 180.0f, 105.0f },
        { "Maluma - Hawai Romantic",            20.0f, 40.0f, 55.0f,  80.0f, 15.0f, 42.0f,  85.0f,  2.0f, 3.5f, 24.0f, 65.0f, 15.0f, 350.0f, 28.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.5f, 220.0f, 110.0f },
        { "Ozuna - Caramelo Tropical",          15.0f, 30.0f, 45.0f,  82.0f, 18.0f, 48.0f,  80.0f,  1.5f, 7.0f, 38.0f, 88.0f, 19.0f, 400.0f, 32.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.0f, 260.0f, 135.0f },
        { "Anuel AA - Real Hasta la Muerte",     0.5f,  3.0f,  8.0f,  97.0f, 55.0f, 78.0f, 110.0f,  2.5f, 5.5f, 13.0f, 42.0f, 17.0f, 260.0f, 38.0f, 0.0f, 100.0f, 2.0f, -44.0f, 1.2f, 150.0f, 105.0f },
        { "Myke Towers - Easy Street",          10.0f, 18.0f, 38.0f,  88.0f, 28.0f, 55.0f,  90.0f,  1.8f, 4.5f, 19.0f, 55.0f, 14.0f, 300.0f, 30.0f, 0.0f, 100.0f, 0.5f, -48.0f, 2.0f, 190.0f, 110.0f },
        { "Jhay Cortez - Tecnologia Future",     1.0f,  2.0f,  5.0f,  98.0f, 38.0f, 68.0f, 105.0f,  2.2f, 6.0f, 24.0f, 60.0f, 21.0f, 220.0f, 42.0f, 0.0f, 100.0f, 1.0f, -46.0f, 1.5f, 170.0f, 120.0f },
        { "Nicky Jam - El Perdon Classic",       8.0f, 15.0f, 35.0f,  88.0f, 32.0f, 58.0f,  92.0f,  1.5f, 3.8f, 17.0f, 52.0f, 12.0f, 270.0f, 26.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.0f, 200.0f, 105.0f },
        { "Daddy Yankee - Gasolina OG",          3.0f,  8.0f, 20.0f,  93.0f, 48.0f, 74.0f,  98.0f,  2.8f, 3.2f, 13.0f, 44.0f, 10.0f, 230.0f, 32.0f, 0.0f, 100.0f, 1.5f, -46.0f, 1.8f, 160.0f, 100.0f },
        { "Sech - Otro Trago Groove",           12.0f, 22.0f, 42.0f,  84.0f, 20.0f, 46.0f,  82.0f,  1.2f, 5.8f, 30.0f, 72.0f, 15.0f, 320.0f, 28.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.5f, 240.0f, 120.0f },
        { "Feid - Ferxxo Neon Dark",             1.5f,  6.0f, 18.0f,  96.0f, 50.0f, 80.0f, 115.0f,  3.5f, 7.5f, 32.0f, 78.0f, 26.0f, 290.0f, 48.0f, 0.0f, 100.0f, 1.0f, -47.0f, 1.6f, 180.0f, 130.0f },
        { "Trueno - Trap Argentino Raw",         0.0f,  2.0f,  8.0f,  99.0f, 60.0f, 82.0f, 120.0f,  4.0f, 4.5f,  9.0f, 38.0f, 13.0f, 200.0f, 30.0f, 0.0f, 100.0f, 2.0f, -44.0f, 1.0f, 140.0f, 100.0f },
        { "Bizarrap - BZRP Session Edge",        0.0f,  0.0f,  0.0f, 100.0f, 55.0f, 85.0f, 125.0f,  4.5f, 8.0f, 21.0f, 58.0f, 24.0f, 180.0f, 55.0f, 0.0f, 100.0f, 1.5f, -44.0f, 0.8f, 130.0f, 110.0f },
        { "C. Tangana - Bare RnB Madrid",       28.0f, 55.0f, 70.0f,  65.0f, 12.0f, 38.0f,  75.0f,  0.8f, 6.5f, 36.0f, 82.0f, 17.0f, 450.0f, 35.0f, 0.0f, 100.0f,-0.5f, -52.0f, 3.5f, 280.0f, 115.0f },
        { "Quevedo - Columbia UK Drill",         1.0f,  4.0f, 12.0f,  96.0f, 48.0f, 76.0f, 118.0f,  3.2f, 5.0f, 15.0f, 45.0f, 19.0f, 210.0f, 40.0f, 0.0f, 100.0f, 1.5f, -46.0f, 1.4f, 160.0f, 110.0f },
        { "Morad - Barcelona Street",            5.0f, 10.0f, 28.0f,  90.0f, 42.0f, 65.0f,  88.0f,  2.0f, 3.0f, 10.0f, 35.0f,  7.0f, 240.0f, 22.0f, 0.0f, 100.0f, 0.5f, -48.0f, 2.0f, 170.0f, 100.0f },
        // === MODERN HIT EXPANSION (22 NUEVOS PRESETS) ===
        { "Central Cee - Doja UK Drill Snap",    0.0f,  1.0f,  5.0f,  99.0f, 58.0f, 80.0f, 120.0f,  2.5f, 6.0f, 12.0f, 40.0f, 15.0f, 210.0f, 32.0f, 0.0f, 100.0f, 1.5f, -45.0f, 1.0f, 140.0f, 110.0f },
        { "Playboi Carti - Whole Lotta Red Vamp",0.0f,  0.0f,  0.0f, 100.0f, 65.0f, 88.0f, 130.0f,  3.0f, 8.5f, 20.0f, 50.0f, 28.0f, 160.0f, 45.0f, 0.0f, 100.0f, 2.0f, -42.0f, 0.8f, 120.0f, 125.0f },
        { "Lil Uzi Vert - XO Tour Llif3 Melodic",2.0f,  5.0f, 12.0f,  96.0f, 40.0f, 72.0f, 100.0f,  1.5f, 7.0f, 26.0f, 68.0f, 22.0f, 320.0f, 40.0f, 0.0f, 100.0f, 0.5f, -48.0f, 1.8f, 200.0f, 125.0f },
        { "Gunna - Drip Season Wun Wave",        1.0f,  3.0f,  8.0f,  98.0f, 35.0f, 68.0f,  95.0f,  2.0f, 5.0f, 22.0f, 60.0f, 18.0f, 270.0f, 36.0f, 0.0f, 100.0f, 0.5f, -47.0f, 2.0f, 190.0f, 115.0f },
        { "Young Thug - Jeffery High Drip",      0.0f,  0.0f,  4.0f, 100.0f, 48.0f, 78.0f, 110.0f,  1.0f, 8.0f, 24.0f, 62.0f, 25.0f, 240.0f, 42.0f, 0.0f, 100.0f, 1.0f, -45.0f, 1.2f, 160.0f, 120.0f },
        { "Juice WRLD - Lucid Dreams Emotion",  10.0f, 18.0f, 35.0f,  88.0f, 25.0f, 58.0f,  85.0f,  1.8f, 4.8f, 28.0f, 72.0f, 16.0f, 340.0f, 35.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.2f, 220.0f, 115.0f },
        { "XXXTENTACION - Moonlight Intimate",   8.0f, 14.0f, 30.0f,  90.0f, 22.0f, 52.0f,  90.0f,  2.2f, 4.0f, 32.0f, 80.0f, 14.0f, 300.0f, 30.0f, 0.0f, 100.0f, 0.0f, -52.0f, 2.5f, 240.0f, 110.0f },
        { "Mora - Microdosis Perreo Espacial",   1.0f,  4.0f, 10.0f,  97.0f, 42.0f, 72.0f, 105.0f,  2.0f, 6.5f, 30.0f, 75.0f, 20.0f, 260.0f, 42.0f, 0.0f, 100.0f, 1.0f, -46.0f, 1.6f, 180.0f, 135.0f },
        { "Chencho Corleone - Plan B Nostalgia", 0.5f,  2.0f,  6.0f,  98.0f, 52.0f, 82.0f, 115.0f,  3.0f, 5.0f, 16.0f, 45.0f, 14.0f, 220.0f, 32.0f, 0.0f, 100.0f, 1.5f, -45.0f, 1.2f, 150.0f, 105.0f },
        { "Eladio Carrion - Sauce Boy Drill",    1.0f,  3.0f,  8.0f,  98.0f, 48.0f, 75.0f, 112.0f,  2.5f, 5.5f, 14.0f, 42.0f, 16.0f, 230.0f, 35.0f, 0.0f, 100.0f, 1.5f, -45.0f, 1.4f, 160.0f, 110.0f },
        { "Peso Pluma - Ella Baila Sola Corridos",16.0f,30.0f, 45.0f,  82.0f, 30.0f, 50.0f,  85.0f,  1.5f, 6.0f, 20.0f, 55.0f, 12.0f, 280.0f, 25.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.5f, 200.0f,  95.0f },
        { "Young Miko - Trap Kitty Sweet Lead",  2.5f,  6.0f, 15.0f,  95.0f, 36.0f, 65.0f,  95.0f,  1.0f, 7.2f, 28.0f, 70.0f, 18.0f, 310.0f, 38.0f, 0.0f, 100.0f, 0.5f, -48.0f, 2.0f, 200.0f, 125.0f },
        { "Karol G - Provenza Sunshine Vocal",   5.0f, 10.0f, 25.0f,  92.0f, 32.0f, 58.0f,  90.0f,  1.5f, 6.5f, 32.0f, 78.0f, 15.0f, 340.0f, 30.0f, 0.0f, 100.0f, 0.5f, -49.0f, 2.2f, 210.0f, 130.0f },
        { "Dua Lipa - Future Nostalgia Disco",  12.0f, 22.0f, 40.0f,  85.0f, 28.0f, 60.0f,  88.0f,  1.0f, 6.0f, 25.0f, 68.0f, 14.0f, 320.0f, 28.0f, 0.0f, 100.0f, 0.0f, -50.0f, 2.5f, 220.0f, 120.0f },
        { "SZA - Snooze R&B Velvet Dream",      22.0f, 42.0f, 60.0f,  75.0f, 15.0f, 45.0f,  80.0f,  2.8f, 5.0f, 35.0f, 85.0f, 18.0f, 420.0f, 35.0f, 0.0f, 100.0f, 0.0f, -52.0f, 3.5f, 280.0f, 120.0f },
        { "Skrillex - Baby Again Vocal Chop",    0.0f,  0.0f,  0.0f, 100.0f, 60.0f, 85.0f, 135.0f,  2.0f, 9.0f, 18.0f, 52.0f, 28.0f, 180.0f, 50.0f, 0.0f, 100.0f, 2.0f, -40.0f, 0.5f, 110.0f, 150.0f },
        { "Fred again.. - Rumble Ambient Lo-Fi", 8.0f, 15.0f, 35.0f,  88.0f, 20.0f, 55.0f, 120.0f,  3.0f, 3.5f, 42.0f, 92.0f, 22.0f, 480.0f, 42.0f, 0.0f, 100.0f,-1.0f, -48.0f, 3.0f, 300.0f, 140.0f },
        { "T-Pain - Extreme 0ms Instant Snap",   0.0f,  0.0f,  0.0f, 100.0f, 40.0f, 75.0f, 100.0f,  2.0f, 5.0f, 12.0f, 40.0f, 10.0f, 240.0f, 25.0f, 0.0f, 100.0f, 1.0f, -46.0f, 1.0f, 150.0f, 100.0f },
        { "Studio - Clean Podcast & Broadcast", 32.0f, 50.0f, 65.0f,  70.0f, 10.0f, 42.0f,  85.0f,  2.5f, 3.0f,  0.0f,  0.0f,  0.0f, 200.0f,  0.0f, 0.0f, 100.0f, 0.0f, -52.0f, 2.5f, 220.0f, 100.0f },
        { "Studio - Radio Megaphone Lo-Fi",      0.0f,  0.0f,  0.0f, 100.0f, 45.0f, 85.0f, 220.0f, -4.0f, 8.5f, 15.0f, 45.0f, 20.0f, 200.0f, 35.0f, 0.0f, 100.0f, 1.5f, -42.0f, 1.0f, 120.0f,  40.0f },
        { "Studio - Acoustic Natural Session",  40.0f, 65.0f, 75.0f,  55.0f,  5.0f, 30.0f,  75.0f,  1.5f, 4.0f, 20.0f, 65.0f, 10.0f, 350.0f, 22.0f, 0.0f, 100.0f, 0.0f, -54.0f, 4.0f, 300.0f, 105.0f },
        { "Studio - Ultra-Wide Vocal Doubler",   1.0f,  4.0f, 10.0f,  96.0f, 35.0f, 60.0f,  90.0f,  1.0f, 6.5f, 28.0f, 75.0f, 24.0f, 280.0f, 40.0f, 0.0f, 100.0f, 0.5f, -48.0f, 2.0f, 200.0f, 185.0f }
    };
    return presets;
}

int TPainSupremeAudioProcessor::getNumPrograms() { return static_cast<int>(getPresets().size()); }
int TPainSupremeAudioProcessor::getCurrentProgram() { return currentProgramIndex; }
void TPainSupremeAudioProcessor::setCurrentProgram(int index) { loadPreset(index); }
const juce::String TPainSupremeAudioProcessor::getProgramName(int index)
{
    const auto& p = getPresets();
    if (index >= 0 && index < static_cast<int>(p.size()))
        return p[static_cast<size_t>(index)].name;
    return "Default";
}
void TPainSupremeAudioProcessor::changeProgramName(int, const juce::String&) {}

void TPainSupremeAudioProcessor::loadPreset(int index)
{
    const auto& presets = getPresets();
    if (index < 0 || index >= static_cast<int>(presets.size())) return;
    currentProgramIndex = index;
    const auto& p = presets[static_cast<size_t>(index)];

    auto setParam = [this](const char* id, float value) {
        if (auto* param = apvts.getParameter(id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    };

    setParam(ID_INPUT, p.inputGain);
    setParam(ID_RETUNE_SPEED, p.retuneSpeed);
    setParam(ID_TRANSITION, p.transition);
    setParam(ID_VARIATION, p.variation);
    setParam(ID_CENTERING, p.centering);
    setParam(ID_TRANSIENTS, p.transients);
    setParam(ID_COMPRESSION, p.compression);
    setParam(ID_LOW_CUT, p.lowCut);
    setParam(ID_BODY, p.body);
    setParam(ID_AIR, p.air);
    setParam(ID_REVERB_MIX, p.reverbMix);
    setParam(ID_REVERB_SIZE, p.reverbSize);
    setParam(ID_DELAY_MIX, p.delayMix);
    setParam(ID_DELAY_TIME, p.delayTime);
    setParam(ID_DELAY_FEEDBACK, p.delayFeedback);
    setParam(ID_STEREO_WIDTH, p.stereoWidth);
    setParam(ID_OUTPUT, p.output);
    setParam(ID_MIX, p.mix);
    setParam(ID_GATE_THRESH, p.gateThresh);
    setParam(ID_GATE_ATTACK, p.gateAttack);
    setParam(ID_GATE_RELEASE, p.gateRelease);
}

void TPainSupremeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    try
    {
        pitchEngine.setSampleRate(sampleRate);
        vocalFX.prepare(sampleRate, samplesPerBlock);

        // Pre-allocate dry buffer fixed to 65536 samples (never reallocates on audio thread)
        dryBuffer.setSize(2, 65536, false, true, true);
        dryBuffer.clear();

        dcBlockerX1 = 0.0f;
        dcBlockerY1 = 0.0f;
        dcBlockerX2 = 0.0f;
        dcBlockerY2 = 0.0f;

        pitchEngine.reset();
        vocalFX.reset();

        setLatencySamples(pitchEngine.getLatencySamples());
    }
    catch (...)
    {
    }
}

void TPainSupremeAudioProcessor::releaseResources()
{
    try
    {
        pitchEngine.reset();
        vocalFX.reset();
        dryBuffer.clear();
        dcBlockerX1 = 0.0f;
        dcBlockerY1 = 0.0f;
        dcBlockerX2 = 0.0f;
        dcBlockerY2 = 0.0f;
    }
    catch (...)
    {
    }
}

bool TPainSupremeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void TPainSupremeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    try
    {
        const int numSamples = buffer.getNumSamples();
        if (numSamples <= 0)
            return;

        const int totalNumInputChannels = getTotalNumInputChannels();
        const int totalNumOutputChannels = getTotalNumOutputChannels();

        if (totalNumInputChannels <= 0)
        {
            buffer.clear();
            return;
        }

        // Clear excess output channels
        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, numSamples);

        // If Mono microphone input in FL Studio, duplicate to channel 1 for true stereo processing
        if (totalNumInputChannels == 1 && totalNumOutputChannels >= 2 && buffer.getNumChannels() >= 2)
        {
            buffer.copyFrom(1, 0, buffer.getReadPointer(0), numSamples);
        }

        const int activeChannels = std::min(buffer.getNumChannels(), std::max(totalNumInputChannels, 1));

        // 0. Input Gain & Real-Time Input Level Monitoring (Gain Staging)
        float inGainDb = paramInput ? paramInput->load() : 0.0f;
        float inGainLin = juce::Decibels::decibelsToGain(inGainDb);
        float inputPeak = 0.0f;

        for (int ch = 0; ch < activeChannels; ++ch)
        {
            float* d = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                d[i] *= inGainLin;
                float absVal = std::fabs(d[i]);
                if (absVal > inputPeak) inputPeak = absVal;
            }
        }
        float inPeakDb = (inputPeak > 1e-4f) ? juce::Decibels::gainToDecibels(inputPeak) : -60.0f;
        float prevInMeter = inputMeterPeakDb.load(std::memory_order_relaxed);
        inputMeterPeakDb.store(prevInMeter * 0.75f + inPeakDb * 0.25f, std::memory_order_relaxed);

        // 1. High-Pass DC Blocker (10 Hz) & Input Safety Limiter (Anti-Explosion & Anti-Pop)
        for (int ch = 0; ch < activeChannels; ++ch)
        {
            float* d = buffer.getWritePointer(ch);
            float& xPrev = (ch == 0) ? dcBlockerX1 : dcBlockerX2;
            float& yPrev = (ch == 0) ? dcBlockerY1 : dcBlockerY2;

            for (int i = 0; i < numSamples; ++i)
            {
                float s = d[i];
                if (!std::isfinite(s)) s = 0.0f;

                // Strip sudden DC transient spikes caused by mic arm/disarm
                float y = s - xPrev + 0.9992f * yPrev;
                if (!std::isfinite(y)) y = 0.0f;
                xPrev = s;
                yPrev = y;

                // Soft-saturate extreme peaks (> +6 dBFS) to prevent IIR filter explosion
                if (y > 2.0f) y = 2.0f + std::tanh(y - 2.0f) * 0.5f;
                else if (y < -2.0f) y = -2.0f + std::tanh(y + 2.0f) * 0.5f;

                d[i] = y;
            }
        }

        // === NOISE GATE (before pitch engine, post DC-block) ===
        {
            float gateThreshDb  = paramGateThresh  ? paramGateThresh->load()  : -50.0f;
            float gateAttackMs  = paramGateAttack  ? paramGateAttack->load()  :   2.0f;
            float gateReleaseMs = paramGateRelease ? paramGateRelease->load() : 200.0f;

            double sr = getSampleRate();
            if (sr <= 0.0) sr = 44100.0;

            // Compute per-sample coefficients
            float gateThreshLin = juce::Decibels::decibelsToGain(gateThreshDb);
            float alphaAtk = (gateAttackMs  < 0.01f) ? 0.0f
                             : std::exp(-1.0f / (static_cast<float>(sr) * gateAttackMs  * 0.001f));
            float alphaRel = (gateReleaseMs < 0.01f) ? 0.0f
                             : std::exp(-1.0f / (static_cast<float>(sr) * gateReleaseMs * 0.001f));

            float blockMinGainLin = 1.0f;
            bool anyOpen = false;

            for (int ch = 0; ch < activeChannels; ++ch)
            {
                float* d = buffer.getWritePointer(ch);
                float& env = gateEnv[ch < 2 ? ch : 1];

                for (int i = 0; i < numSamples; ++i)
                {
                    float level = std::fabs(d[i]);
                    // Envelope follower: fast attack, slow release
                    float alpha = (level > env) ? alphaAtk : alphaRel;
                    env = alpha * env + (1.0f - alpha) * level;
                    if (!std::isfinite(env)) env = 0.0f;

                    // Gate: apply attenuation below threshold
                    if (env < gateThreshLin)
                    {
                        // Soft knee: ratio proportional to how far below threshold
                        float ratio = (gateThreshLin > 0.0f) ? (env / gateThreshLin) : 0.0f;
                        float gain = ratio * ratio; // quadratic taper — smooth, no click
                        d[i] *= gain;
                        if (gain < blockMinGainLin)
                            blockMinGainLin = gain;
                    }
                    else
                    {
                        anyOpen = true;
                    }
                }
            }

            float currentBlockGrDb = 0.0f;
            if (blockMinGainLin < 0.99f)
            {
                float db = -juce::Decibels::gainToDecibels(std::max(blockMinGainLin, 1e-3f));
                currentBlockGrDb = juce::jlimit(0.0f, 60.0f, db);
            }

            float prevGr = gateGainReductionDb.load(std::memory_order_relaxed);
            float smoothGr = prevGr * 0.75f + currentBlockGrDb * 0.25f;
            gateGainReductionDb.store(smoothGr, std::memory_order_relaxed);
            gateIsOpen.store(anyOpen || (currentBlockGrDb < 2.0f), std::memory_order_relaxed);
        }

        // Retrieve active parameters with null protection
        int rootIdx = paramRoot ? static_cast<int>(paramRoot->load()) : 0;
        int scaleIdx = paramScale ? static_cast<int>(paramScale->load()) : 0;
        float speedMs = paramRetuneSpeed ? paramRetuneSpeed->load() : 0.0f;
        float variation = paramVariation ? paramVariation->load() : 0.0f;
        float centering = paramCentering ? paramCentering->load() : 100.0f;
        float transition = paramTransition ? paramTransition->load() : 0.0f;
        float transients = paramTransients ? paramTransients->load() : 0.0f;
        float compression = paramCompression ? paramCompression->load() : 30.0f;
        float lowCut = paramLowCut ? paramLowCut->load() : 80.0f;
        float body = paramBody ? paramBody->load() : 0.0f;
        float air = paramAir ? paramAir->load() : 3.0f;
        float reverbMix = paramReverbMix ? paramReverbMix->load() : 15.0f;
        float reverbSize = paramReverbSize ? paramReverbSize->load() : 50.0f;
        float delayMix = paramDelayMix ? paramDelayMix->load() : 0.0f;
        float delayTime = paramDelayTime ? paramDelayTime->load() : 320.0f;
        float delayFeedback = paramDelayFeedback ? paramDelayFeedback->load() : 35.0f;
        float outputDb = paramOutput ? paramOutput->load() : 0.0f;
        float mixPercent = paramMix ? paramMix->load() : 100.0f;

        VeviAudio::ScaleType scaleType = VeviAudio::ScaleType::Major;
        switch (scaleIdx)
        {
            case 0: scaleType = VeviAudio::ScaleType::Major; break;
            case 1: scaleType = VeviAudio::ScaleType::NaturalMinor; break;
            case 2: scaleType = VeviAudio::ScaleType::HarmonicMinor; break;
            case 3: scaleType = VeviAudio::ScaleType::MelodicMinor; break;
            case 4: scaleType = VeviAudio::ScaleType::PentatonicMajor; break;
            case 5: scaleType = VeviAudio::ScaleType::PentatonicMinor; break;
            case 6: scaleType = VeviAudio::ScaleType::Blues; break;
            case 7: scaleType = VeviAudio::ScaleType::Chromatic; break;
            default: scaleType = VeviAudio::ScaleType::Major; break;
        }

        pitchEngine.setRootNote(rootIdx);
        pitchEngine.setScaleType(scaleType);
        pitchEngine.setRetuneSpeedMs(speedMs);
        pitchEngine.setVariation(variation);
        pitchEngine.setCentering(centering);
        pitchEngine.setTransitionMs(transition);

        vocalFX.setTransientAmount(transients);
        vocalFX.setCompression(compression);
        vocalFX.setLowCut(lowCut);
        vocalFX.setBodyGain(body);
        vocalFX.setAirGain(air);
        vocalFX.setReverbMix(reverbMix);
        vocalFX.setReverbSize(reverbSize);
        vocalFX.setDelayMix(delayMix);
        vocalFX.setDelayTimeMs(delayTime);
        vocalFX.setDelayFeedback(delayFeedback);
        vocalFX.setOutputGainDb(outputDb);
        vocalFX.setMix(mixPercent);

        // Save dry buffer if mix is not 100%
        if (mixPercent < 99.9f)
        {
            int safeSamples = std::min(numSamples, dryBuffer.getNumSamples());
            for (int ch = 0; ch < activeChannels && ch < dryBuffer.getNumChannels(); ++ch)
                dryBuffer.copyFrom(ch, 0, buffer, ch, 0, safeSamples);
        }

        // 2. Process Pitch Correction (T-Pain Engine)
        float* channelDataL = buffer.getWritePointer(0);
        float* channelDataR = (buffer.getNumChannels() > 1) ? buffer.getWritePointer(1) : nullptr;

        pitchEngine.process(channelDataL, channelDataR, channelDataL, channelDataR, numSamples);

        // 3. Process Vocal FX Chain (Transients + Compression + Light EQ + Space FX + Saturation)
        vocalFX.process(buffer);

        // 4. Dry / Wet blend if requested
        if (mixPercent < 99.9f)
        {
            float wetRatio = juce::jlimit(0.0f, 1.0f, mixPercent / 100.0f);
            float dryRatio = 1.0f - wetRatio;
            int safeSamples = std::min(numSamples, dryBuffer.getNumSamples());

            for (int ch = 0; ch < activeChannels && ch < dryBuffer.getNumChannels(); ++ch)
            {
                float* wet = buffer.getWritePointer(ch);
                const float* dry = dryBuffer.getReadPointer(ch);
                for (int i = 0; i < safeSamples; ++i)
                    wet[i] = dryRatio * dry[i] + wetRatio * wet[i];
            }
        }

        // If input was Mono but track is Stereo, ensure both channels output the perfected audio
        if (totalNumInputChannels == 1 && totalNumOutputChannels >= 2 && buffer.getNumChannels() >= 2)
        {
            buffer.copyFrom(1, 0, buffer.getReadPointer(0), numSamples);
        }

        // 5. Stereo Width Processing (Mid/Side)
        float stereoWidthPercent = paramStereoWidth ? paramStereoWidth->load() : 100.0f;
        if (buffer.getNumChannels() >= 2)
        {
            float widthRatio = juce::jlimit(0.0f, 2.0f, stereoWidthPercent / 100.0f);
            float* l = buffer.getWritePointer(0);
            float* r = buffer.getWritePointer(1);
            for (int i = 0; i < numSamples; ++i)
            {
                float mid = 0.5f * (l[i] + r[i]);
                float side = 0.5f * (l[i] - r[i]) * widthRatio;
                l[i] = mid + side;
                r[i] = mid - side;
            }
        }

        // 6. Real-Time Output Level Monitoring (Gain Staging)
        float outputPeak = 0.0f;
        for (int ch = 0; ch < activeChannels && ch < buffer.getNumChannels(); ++ch)
        {
            const float* d = buffer.getReadPointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float absVal = std::fabs(d[i]);
                if (absVal > outputPeak) outputPeak = absVal;
            }
        }
        float outPeakDb = (outputPeak > 1e-4f) ? juce::Decibels::gainToDecibels(outputPeak) : -60.0f;
        float prevOutMeter = outputMeterPeakDb.load(std::memory_order_relaxed);
        outputMeterPeakDb.store(prevOutMeter * 0.75f + outPeakDb * 0.25f, std::memory_order_relaxed);
    }
    catch (...)
    {
        // Absolute crash shield
    }
}

bool TPainSupremeAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* TPainSupremeAudioProcessor::createEditor()
{
    return new TPainSupremeAudioProcessorEditor(*this);
}

void TPainSupremeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    try
    {
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        if (xml != nullptr)
            copyXmlToBinary(*xml, destData);
    }
    catch (...)
    {
    }
}

void TPainSupremeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    try
    {
        if (data == nullptr || sizeInBytes <= 0)
            return;

        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
    catch (...)
    {
    }
}

// Juce plugin entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TPainSupremeAudioProcessor();
}
