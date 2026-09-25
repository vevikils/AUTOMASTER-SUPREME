#include "PluginProcessor.h"
#include "PluginEditor.h"

VeviProQAudioProcessor::VeviProQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout()),
      forwardFFT (FFT_ORDER),
      window (FFT_SIZE, juce::dsp::WindowingFunction<float>::hann)
#endif
{
    preFifo.fill(0.0f);
    preFftData.fill(0.0f);
    postFifo.fill(0.0f);
    postFftData.fill(0.0f);

    paramMasterGain   = apvts.getRawParameterValue("master_gain");
    paramGlobalBypass = apvts.getRawParameterValue("global_bypass");
    paramPhaseMode    = apvts.getRawParameterValue("phase_mode");

    for (int i = 0; i < NUM_BANDS; ++i)
    {
        bandParams[i].enabled       = apvts.getRawParameterValue(getParamId(i, "enable"));
        bandParams[i].type          = apvts.getRawParameterValue(getParamId(i, "type"));
        bandParams[i].freq          = apvts.getRawParameterValue(getParamId(i, "freq"));
        bandParams[i].gain          = apvts.getRawParameterValue(getParamId(i, "gain"));
        bandParams[i].q             = apvts.getRawParameterValue(getParamId(i, "q"));
        bandParams[i].dynEnabled    = apvts.getRawParameterValue(getParamId(i, "dyn_enable"));
        bandParams[i].dynRange      = apvts.getRawParameterValue(getParamId(i, "dyn_range"));
        bandParams[i].threshold     = apvts.getRawParameterValue(getParamId(i, "dyn_thresh"));
        bandParams[i].slope         = apvts.getRawParameterValue(getParamId(i, "slope"));
        bandParams[i].stereoRouting = apvts.getRawParameterValue(getParamId(i, "routing"));
    }
}

VeviProQAudioProcessor::~VeviProQAudioProcessor()
{
}

const std::vector<VeviProQAudioProcessor::ArtistPreset>& VeviProQAudioProcessor::getArtistPresets()
{
    static const std::vector<ArtistPreset> presets = {
        {
            "Default (Flat)",
            "Init",
            {{
                { true, 0,    32.0f, 0.0f, 0.707f, false, 0.0f, -20.0f, 1, 0 },
                { true, 0,   100.0f, 0.0f, 0.707f, false, 0.0f, -20.0f, 1, 0 },
                { true, 0,   450.0f, 0.0f, 1.000f, false, 0.0f, -20.0f, 1, 0 },
                { true, 0,  1230.0f, 0.0f, 1.000f, false, 0.0f, -20.0f, 1, 0 },
                { true, 0,  2600.0f, 0.0f, 1.000f, false, 0.0f, -20.0f, 1, 0 },
                { true, 0,  6000.0f, 0.0f, 0.707f, false, 0.0f, -20.0f, 1, 0 },
                { true, 0, 13000.0f, 0.0f, 0.707f, false, 0.0f, -20.0f, 1, 0 }
            }},
            0.0f
        },
        {
            "FL Studio - Master Polish",
            "Mastering",
            {{
                { true, 3,  32.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // LowCut 24dB
                { true, 1, 100.0f,   -1.3f,  0.71f, false,  0.0f, -20.0f, 4, 0 }, // LowShelf
                { true, 0, 450.0f,    1.8f,  1.20f, false,  0.0f, -20.0f, 1, 0 }, // Bell
                { true, 0, 1230.0f,   2.5f,  2.20f, true,   2.0f, -20.0f, 1, 0 }, // Bell Dyn
                { true, 0, 2600.0f,   0.5f,  1.40f, false,  0.0f, -20.0f, 1, 0 }, // Bell
                { true, 0, 6000.0f,  -0.5f,  1.40f, false,  0.0f, -20.0f, 1, 0 }, // Bell
                { true, 2, 13000.0f,  1.8f,  0.71f, false,  0.0f, -20.0f, 1, 0 }  // HighShelf
            }},
            0.0f
        },
        {
            "Travis Scott - Astroworld Vocals",
            "Vocals",
            {{
                { true, 3,  95.0f,    0.0f,  0.85f, false,  0.0f, -20.0f, 3, 0 }, // LowCut 24dB
                { true, 0, 280.0f,   -2.5f,  1.80f, false,  0.0f, -20.0f, 1, 0 }, // Clean mud
                { true, 0, 850.0f,   -1.8f,  2.20f, false,  0.0f, -20.0f, 1, 0 }, // Boxiness dip
                { true, 0, 3100.0f,   2.8f,  1.60f, true,  -2.5f, -18.0f, 1, 0 }, // In-your-face dynamic presence
                { true, 0, 6800.0f,  -2.5f,  3.50f, true,  -3.0f, -16.0f, 1, 0 }, // Harsh sibilance de-esser
                { true, 2, 11500.0f,  3.0f,  0.75f, false,  0.0f, -20.0f, 1, 0 }, // Glossy top shelf
                { true, 4, 19500.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 2, 0 }  // HighCut
            }},
            -0.5f
        },
        {
            "Metro Boomin - 808 & Kick Tighten",
            "Drums & Bass",
            {{
                { true, 3,  28.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 4, 0 }, // LowCut 48dB ultra-sub
                { true, 0,  55.0f,    2.8f,  1.90f, true,   2.0f, -14.0f, 1, 0 }, // Deep sub bass punch
                { true, 0, 160.0f,   -3.0f,  2.40f, false,  0.0f, -20.0f, 1, 0 }, // De-mud 808
                { true, 0, 750.0f,    2.2f,  1.80f, false,  0.0f, -20.0f, 1, 0 }, // 808 grit
                { true, 0, 2400.0f,  -1.8f,  1.20f, false,  0.0f, -20.0f, 1, 0 }, // Tame bite
                { true, 0, 4800.0f,   1.5f,  1.50f, false,  0.0f, -20.0f, 1, 0 }, // Top click
                { true, 4, 15000.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }  // HighCut
            }},
            -0.5f
        },
        {
            "Billie Eilish - Intimate Air Vocal",
            "Vocals",
            {{
                { true, 3, 110.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 2, 0 }, // Clean low rumble
                { true, 1, 220.0f,    1.5f,  0.80f, false,  0.0f, -20.0f, 1, 0 }, // Intimate chest warmth
                { true, 0, 500.0f,   -1.8f,  1.50f, false,  0.0f, -20.0f, 1, 0 }, // Hollow room removal
                { true, 0, 2800.0f,  -1.2f,  2.00f, true,  -2.5f, -22.0f, 1, 0 }, // Dynamic soothe
                { true, 0, 7200.0f,  -2.4f,  3.50f, true,  -3.5f, -18.0f, 1, 0 }, // Dynamic sibilance tamer
                { true, 2, 12000.0f,  2.9f,  0.65f, false,  0.0f, -20.0f, 1, 0 }, // Whisper breath air
                { true, 4, 21000.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 1, 0 }  // High Cut
            }},
            0.0f
        },
        {
            "Daft Punk - Master Bus Clarity & Punch",
            "Mastering",
            {{
                { true, 3,  25.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // LowCut 24dB
                { true, 1,  80.0f,    1.8f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Solid bottom shelf
                { true, 0, 320.0f,   -1.5f,  1.60f, false,  0.0f, -20.0f, 1, 0 }, // Mid clarity pocket
                { true, 0, 1100.0f,   1.2f,  1.20f, false,  0.0f, -20.0f, 1, 0 }, // Analog vintage body
                { true, 0, 3500.0f,   1.8f,  1.40f, true,   1.5f, -16.0f, 1, 0 }, // Snappy transients
                { true, 0, 8500.0f,   1.6f,  1.10f, false,  0.0f, -20.0f, 1, 0 }, // Disco sheen
                { true, 2, 16000.0f,  2.2f,  0.71f, false,  0.0f, -20.0f, 1, 0 }  // Silky air
            }},
            -0.2f
        },
        {
            "Martin Garrix - EDM Festival Lead",
            "Synths & Leads",
            {{
                { true, 3, 140.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // LowCut lead cleanup
                { true, 0, 350.0f,   -2.2f,  1.50f, false,  0.0f, -20.0f, 1, 0 }, // Clean bass collision
                { true, 0, 1200.0f,   2.0f,  1.80f, false,  0.0f, -20.0f, 1, 0 }, // Synth core tone
                { true, 0, 3200.0f,   3.0f,  1.50f, true,   2.0f, -12.0f, 1, 0 }, // Aggressive lead cut-through
                { true, 0, 6500.0f,   1.8f,  2.00f, false,  0.0f, -20.0f, 1, 0 }, // High mid bite
                { true, 2, 10000.0f,  2.8f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Crisp top shelf
                { true, 4, 18000.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 2, 0 }  // HighCut
            }},
            -0.5f
        },
        {
            "Drake - Warm OVO R&B Mix",
            "Mixing",
            {{
                { true, 3,  30.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // LowCut
                { true, 1,  90.0f,    2.5f,  0.80f, false,  0.0f, -20.0f, 1, 0 }, // Heavy warm low end
                { true, 0, 400.0f,   -1.8f,  1.40f, false,  0.0f, -20.0f, 1, 0 }, // De-mud
                { true, 0, 1800.0f,   1.2f,  1.10f, false,  0.0f, -20.0f, 1, 0 }, // Radio vibe
                { true, 0, 4200.0f,  -1.5f,  1.60f, true,  -2.0f, -20.0f, 1, 0 }, // Smooth tamer
                { true, 2, 10500.0f,  2.0f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Warm gloss
                { true, 4, 17500.0f, -1.0f,  0.71f, false,  0.0f, -20.0f, 1, 0 }  // Analog roll-off
            }},
            -0.5f
        },
        {
            "The Weeknd - After Hours 80s Synthwave",
            "Synths & Leads",
            {{
                { true, 3,  35.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // LowCut
                { true, 1,  95.0f,    2.2f,  0.80f, false,  0.0f, -20.0f, 1, 0 }, // Analog bass warmth
                { true, 0, 480.0f,   -2.0f,  1.50f, false,  0.0f, -20.0f, 1, 0 }, // Clear synth clutter
                { true, 0, 2200.0f,   1.8f,  1.30f, false,  0.0f, -20.0f, 1, 0 }, // Vintage Juno brass
                { true, 0, 4500.0f,   2.0f,  1.20f, true,  -1.5f, -18.0f, 1, 0 }, // Dynamic synth presence
                { true, 2, 12500.0f,  2.5f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // High sparkle shelf
                { true, 4, 19000.0f, -0.8f,  0.71f, false,  0.0f, -20.0f, 2, 0 }  // HighCut
            }},
            -0.2f
        },
        {
            "Skrillex - Modern Bass & Drum Crunch",
            "Drums & Bass",
            {{
                { true, 3,  30.0f,    0.0f,  0.85f, false,  0.0f, -20.0f, 4, 0 }, // Ultra sharp cut
                { true, 0,  70.0f,    2.6f,  2.20f, true,   2.5f, -15.0f, 1, 0 }, // Sub slam
                { true, 0, 230.0f,   -3.2f,  2.80f, false,  0.0f, -20.0f, 1, 0 }, // Remove mud for snares
                { true, 0, 1100.0f,   2.4f,  2.50f, false,  0.0f, -20.0f, 1, 0 }, // Metallic bass growl
                { true, 0, 3600.0f,   2.8f,  1.80f, true,  -2.0f, -14.0f, 1, 0 }, // Aggressive snare snap
                { true, 2,  8500.0f,  1.5f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Bright top shelf
                { true, 4, 18500.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }  // High cut
            }},
            -0.5f
        },
        {
            "Dua Lipa - Disco Funk Bass & Punch",
            "Drums & Bass",
            {{
                { true, 3,  38.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // Tight low end
                { true, 1, 105.0f,    2.4f,  0.90f, false,  0.0f, -20.0f, 1, 0 }, // Round funk bottom
                { true, 0, 360.0f,   -2.4f,  1.70f, false,  0.0f, -20.0f, 1, 0 }, // Clean bass guitar pocket
                { true, 0, 1400.0f,   1.8f,  1.60f, false,  0.0f, -20.0f, 1, 0 }, // Finger plucking tone
                { true, 0, 3800.0f,   2.2f,  1.40f, true,   1.5f, -16.0f, 1, 0 }, // Snappy transient pop
                { true, 2, 13500.0f,  2.0f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Studio sheen
                { true, 4, 20000.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 1, 0 }  // HighCut
            }},
            -0.2f
        },
        {
            "Kendrick Lamar - West Coast Vocal Depth",
            "Vocals",
            {{
                { true, 3,  90.0f,    0.0f,  0.85f, false,  0.0f, -20.0f, 3, 0 }, // HPF
                { true, 0, 240.0f,   -2.2f,  1.60f, false,  0.0f, -20.0f, 1, 0 }, // De-mud
                { true, 0, 1200.0f,   1.5f,  1.30f, false,  0.0f, -20.0f, 1, 0 }, // Authentic throat grit
                { true, 0, 2800.0f,   2.6f,  1.60f, true,  -2.0f, -16.0f, 1, 0 }, // Aggressive lyric clarity
                { true, 0, 6400.0f,  -2.6f,  3.20f, true,  -3.5f, -18.0f, 1, 0 }, // De-esser
                { true, 2, 11000.0f,  2.2f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Crisp air
                { true, 4, 19000.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 2, 0 }  // HighCut
            }},
            -0.3f
        },
        {
            "Post Malone - Melodic Reverb & Air",
            "Vocals",
            {{
                { true, 3,  85.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // LowCut
                { true, 1, 200.0f,    1.8f,  0.80f, false,  0.0f, -20.0f, 1, 0 }, // Warm body
                { true, 0, 600.0f,   -2.0f,  1.60f, false,  0.0f, -20.0f, 1, 0 }, // Remove nasal build
                { true, 0, 2400.0f,   2.2f,  1.40f, true,   1.8f, -16.0f, 1, 0 }, // Bright melodic cut
                { true, 0, 5600.0f,  -2.0f,  2.80f, true,  -3.0f, -18.0f, 1, 0 }, // Dynamic sibilance control
                { true, 2, 14000.0f,  2.8f,  0.65f, false,  0.0f, -20.0f, 1, 0 }, // Heavenly reverb sheen
                { true, 4, 20500.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 1, 0 }  // HighCut
            }},
            -0.2f
        },
        {
            "Calvin Harris - Summer Festival Master",
            "Mastering",
            {{
                { true, 3,  28.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 4, 0 }, // Sub cut
                { true, 1,  65.0f,    2.2f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Club low punch
                { true, 0, 260.0f,   -1.8f,  1.60f, false,  0.0f, -20.0f, 1, 0 }, // Mid clarity
                { true, 0, 1600.0f,   1.4f,  1.20f, false,  0.0f, -20.0f, 1, 0 }, // Energy mid
                { true, 0, 4200.0f,   1.8f,  1.40f, true,   1.2f, -16.0f, 1, 0 }, // Vocal & synth cut
                { true, 2, 10500.0f,  2.5f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Bright gloss
                { true, 2, 16000.0f,  1.8f,  0.71f, false,  0.0f, -20.0f, 1, 0 }  // Air sheen
            }},
            -0.3f
        },
        {
            "Bad Bunny - Reggaeton Dembow & Bass",
            "Drums & Bass",
            {{
                { true, 3,  32.0f,    0.0f,  0.85f, false,  0.0f, -20.0f, 4, 0 }, // Clean sub rumble
                { true, 0,  62.0f,    2.8f,  1.80f, true,   2.2f, -14.0f, 1, 0 }, // Heavy 808 kick punch
                { true, 0, 190.0f,   -2.8f,  2.20f, false,  0.0f, -20.0f, 1, 0 }, // Mud removal for kick
                { true, 0, 950.0f,    2.0f,  1.80f, false,  0.0f, -20.0f, 1, 0 }, // Dembow snare slap
                { true, 0, 3100.0f,   1.6f,  1.50f, false,  0.0f, -20.0f, 1, 0 }, // Vocal rhythm presence
                { true, 2,  9500.0f,  2.4f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Crisp hi-hat shelf
                { true, 4, 18000.0f, -1.0f,  0.71f, false,  0.0f, -20.0f, 2, 0 }  // Clean high roll
            }},
            -0.4f
        },
        {
            "Bizarrap - Bzrp Session Punch & Bite",
            "Mastering",
            {{
                { true, 3,  30.0f,    0.0f,  0.85f, false,  0.0f, -20.0f, 3, 0 }, // Fast HPF
                { true, 1,  85.0f,    2.4f,  0.85f, false,  0.0f, -20.0f, 1, 0 }, // Hard transient punch
                { true, 0, 340.0f,   -2.5f,  1.80f, false,  0.0f, -20.0f, 1, 0 }, // Clean center pocket
                { true, 0, 1300.0f,   1.8f,  1.50f, false,  0.0f, -20.0f, 1, 0 }, // Raw beat presence
                { true, 0, 3300.0f,   2.6f,  1.50f, true,  -2.0f, -16.0f, 1, 0 }, // Cutting vocal aggression
                { true, 2, 11500.0f,  2.8f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Modern hyper-sheen
                { true, 4, 19500.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 2, 0 }  // HighCut
            }},
            -0.3f
        },
        {
            "Avicii - Euphoric Progressive Synth",
            "Synths & Leads",
            {{
                { true, 3,  80.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // Clean bass space
                { true, 0, 280.0f,   -2.0f,  1.60f, false,  0.0f, -20.0f, 1, 0 }, // De-cloud
                { true, 0, 1100.0f,   2.0f,  1.40f, false,  0.0f, -20.0f, 1, 0 }, // Big melodic piano body
                { true, 0, 2900.0f,   2.5f,  1.40f, true,   1.8f, -16.0f, 1, 0 }, // Supersaw bite
                { true, 0, 5500.0f,   1.8f,  1.60f, false,  0.0f, -20.0f, 1, 0 }, // Lead top energy
                { true, 2, 12000.0f,  2.8f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Euphoric anthem air
                { true, 4, 18500.0f,  0.0f,  0.71f, false,  0.0f, -20.0f, 2, 0 }  // HighCut
            }},
            -0.3f
        },
        {
            "Hans Zimmer - Cinematic Sub & Epic Air",
            "Mastering",
            {{
                { true, 3,  22.0f,    0.0f,  0.71f, false,  0.0f, -20.0f, 3, 0 }, // Deep infrasonic protection
                { true, 1,  50.0f,    2.8f,  0.65f, false,  0.0f, -20.0f, 1, 0 }, // Massive taiko sub impact
                { true, 0, 220.0f,   -1.8f,  1.40f, false,  0.0f, -20.0f, 1, 0 }, // Clear orchestral mud
                { true, 0, 850.0f,    1.2f,  1.20f, false,  0.0f, -20.0f, 1, 0 }, // French horn warmth
                { true, 0, 3200.0f,   1.6f,  1.50f, true,   1.4f, -18.0f, 1, 0 }, // Brass & string bite
                { true, 2,  9500.0f,  2.0f,  0.71f, false,  0.0f, -20.0f, 1, 0 }, // Orchestral brilliance
                { true, 2, 15000.0f,  2.6f,  0.60f, false,  0.0f, -20.0f, 1, 0 }  // Epic hall air
            }},
            -0.2f
        }
    };
    return presets;
}

void VeviProQAudioProcessor::loadPreset(int presetIndex)
{
    const auto& presets = getArtistPresets();
    if (presetIndex < 0 || presetIndex >= (int)presets.size())
        return;

    currentPresetIndex = presetIndex;
    const auto& p = presets[(size_t)presetIndex];

    for (int i = 0; i < NUM_BANDS; ++i)
    {
        const auto& b = p.bands[(size_t)i];
        if (auto* param = apvts.getParameter(getParamId(i, "enable")))
            param->setValueNotifyingHost(param->convertTo0to1(b.enabled ? 1.0f : 0.0f));

        if (auto* param = apvts.getParameter(getParamId(i, "type")))
            param->setValueNotifyingHost(param->convertTo0to1((float)b.shape));

        if (auto* param = apvts.getParameter(getParamId(i, "freq")))
            param->setValueNotifyingHost(param->convertTo0to1(b.freq));

        if (auto* param = apvts.getParameter(getParamId(i, "gain")))
            param->setValueNotifyingHost(param->convertTo0to1(b.gain));

        if (auto* param = apvts.getParameter(getParamId(i, "q")))
            param->setValueNotifyingHost(param->convertTo0to1(b.q));

        if (auto* param = apvts.getParameter(getParamId(i, "dyn_enable")))
            param->setValueNotifyingHost(param->convertTo0to1(b.dynEnabled ? 1.0f : 0.0f));

        if (auto* param = apvts.getParameter(getParamId(i, "dyn_range")))
            param->setValueNotifyingHost(param->convertTo0to1(b.dynRange));

        if (auto* param = apvts.getParameter(getParamId(i, "dyn_thresh")))
            param->setValueNotifyingHost(param->convertTo0to1(b.dynThresh));

        if (auto* param = apvts.getParameter(getParamId(i, "slope")))
            param->setValueNotifyingHost(param->convertTo0to1((float)b.slope));

        if (auto* param = apvts.getParameter(getParamId(i, "routing")))
            param->setValueNotifyingHost(param->convertTo0to1((float)b.routing));
    }

    if (auto* masterParam = apvts.getParameter("master_gain"))
        masterParam->setValueNotifyingHost(masterParam->convertTo0to1(p.masterGain));
}

const juce::String VeviProQAudioProcessor::getName() const { return "VEVI Pro-Q"; }
bool VeviProQAudioProcessor::acceptsMidi() const { return false; }
bool VeviProQAudioProcessor::producesMidi() const { return false; }
bool VeviProQAudioProcessor::isMidiEffect() const { return false; }
double VeviProQAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int VeviProQAudioProcessor::getNumPrograms() { return (int)getArtistPresets().size(); }
int VeviProQAudioProcessor::getCurrentProgram() { return currentPresetIndex; }
void VeviProQAudioProcessor::setCurrentProgram (int index) { loadPreset(index); }
const juce::String VeviProQAudioProcessor::getProgramName (int index)
{
    const auto& presets = getArtistPresets();
    if (index >= 0 && index < (int)presets.size())
        return presets[(size_t)index].name;
    return {};
}
void VeviProQAudioProcessor::changeProgramName (int, const juce::String&) {}

void VeviProQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dspEngine.prepare(sampleRate, samplesPerBlock);
    preFifo.fill(0.0f);
    preFftData.fill(0.0f);
    preFifoIndex = 0;
    postFifo.fill(0.0f);
    postFftData.fill(0.0f);
    postFifoIndex = 0;
    nextFFTBlockReady.store(false);
}

void VeviProQAudioProcessor::releaseResources()
{
    dspEngine.reset();
}

bool VeviProQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void VeviProQAudioProcessor::pushPreSampleIntoFifo(float sample) noexcept
{
    if (preFifoIndex == FFT_SIZE)
    {
        if (!nextFFTBlockReady.load())
        {
            std::fill(preFftData.begin(), preFftData.end(), 0.0f);
            std::copy(preFifo.begin(), preFifo.end(), preFftData.begin());
        }
        preFifoIndex = 0;
    }
    preFifo[(size_t)preFifoIndex++] = sample;
}

void VeviProQAudioProcessor::pushPostSampleIntoFifo(float sample) noexcept
{
    if (postFifoIndex == FFT_SIZE)
    {
        if (!nextFFTBlockReady.load())
        {
            std::fill(postFftData.begin(), postFftData.end(), 0.0f);
            std::copy(postFifo.begin(), postFifo.end(), postFftData.begin());
            nextFFTBlockReady.store(true);
        }
        postFifoIndex = 0;
    }
    postFifo[(size_t)postFifoIndex++] = sample;
}

VeviDSP::BandSettings VeviProQAudioProcessor::getBandSettings(int bandIdx) const
{
    VeviDSP::BandSettings s;
    if (bandIdx >= 0 && bandIdx < NUM_BANDS)
    {
        const auto& p = bandParams[bandIdx];
        s.enabled        = p.enabled ? (p.enabled->load() > 0.5f) : true;
        s.type           = p.type ? static_cast<VeviDSP::FilterType>((int)p.type->load()) : VeviDSP::FilterType::Bell;
        s.frequency      = p.freq ? p.freq->load() : 1000.0f;
        s.gainDB         = p.gain ? p.gain->load() : 0.0f;
        s.q              = p.q ? p.q->load() : 1.0f;
        s.dynamicEnabled = p.dynEnabled ? (p.dynEnabled->load() > 0.5f) : false;
        s.dynamicRangeDB = p.dynRange ? p.dynRange->load() : 0.0f;
        s.thresholdDB    = p.threshold ? p.threshold->load() : -20.0f;
        s.slope          = p.slope ? (int)p.slope->load() : 12;
        s.stereoRouting  = p.stereoRouting ? (int)p.stereoRouting->load() : 0;
    }
    return s;
}

double VeviProQAudioProcessor::getMagnitudeForFrequency(double freq) const
{
    std::array<VeviDSP::BandSettings, NUM_BANDS> currentSettings;
    for (int i = 0; i < NUM_BANDS; ++i)
        currentSettings[i] = getBandSettings(i);

    return dspEngine.getMagnitudeForFrequency(freq, currentSettings);
}

double VeviProQAudioProcessor::getBandMagnitudeForFrequency(int bandIdx, double freq) const
{
    if (bandIdx >= 0 && bandIdx < NUM_BANDS)
    {
        auto s = getBandSettings(bandIdx);
        return dspEngine.getBandMagnitudeForFrequency(bandIdx, freq, s);
    }
    return 1.0;
}

double VeviProQAudioProcessor::getBandMagnitudeWithGain(int bandIdx, double freq, float gainDB) const
{
    if (bandIdx >= 0 && bandIdx < NUM_BANDS)
    {
        auto s = getBandSettings(bandIdx);
        return dspEngine.getBandMagnitudeWithGain(bandIdx, freq, s, gainDB);
    }
    return 1.0;
}

void VeviProQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const bool bypass = paramGlobalBypass ? (paramGlobalBypass->load() > 0.5f) : false;
    if (bypass) return;

    const int numSamples = buffer.getNumSamples();
    if (numSamples == 0) return;

    // 1. Push Raw Input Signal to Pre-EQ FIFO
    const float* preChannelData = buffer.getReadPointer(0);
    for (int i = 0; i < numSamples; ++i)
        pushPreSampleIntoFifo(preChannelData[i]);

    // 2. Perform 7-Band Dynamic EQ & Master Gain Processing
    std::array<VeviDSP::BandSettings, NUM_BANDS> currentSettings;
    for (int i = 0; i < NUM_BANDS; ++i)
        currentSettings[i] = getBandSettings(i);

    const float masterGain = paramMasterGain ? paramMasterGain->load() : 0.0f;
    dspEngine.process(buffer, currentSettings, masterGain);

    // 3. Push Processed Output Signal to Post-EQ FIFO
    const float* postChannelData = buffer.getReadPointer(0);
    for (int i = 0; i < numSamples; ++i)
        pushPostSampleIntoFifo(postChannelData[i]);
}

juce::AudioProcessorValueTreeState::ParameterLayout VeviProQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Master Output & Global Controls
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "master_gain", "Master Output",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        "global_bypass", "Bypass", false));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "phase_mode", "Phase Mode",
        juce::StringArray { "Zero Latency", "Natural Phase", "Linear Phase" }, 1));

    // Preset defaults for 7 bands (Completely flat initial curve on startup)
    const float defaultFreqs[NUM_BANDS] = { 32.0f, 100.0f, 450.0f, 1230.0f, 2600.0f, 6000.0f, 13000.0f };
    const float defaultGains[NUM_BANDS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    const float defaultQs[NUM_BANDS]    = { 0.707f, 0.707f, 1.0f, 1.0f, 1.0f, 0.707f, 0.707f };
    const int defaultTypes[NUM_BANDS]   = { 0, 0, 0, 0, 0, 0, 0 }; // All Bell (flat 0dB)

    for (int i = 0; i < NUM_BANDS; ++i)
    {
        const juce::String prefix = "band" + juce::String(i + 1) + "_";

        layout.add(std::make_unique<juce::AudioParameterBool>(
            prefix + "enable", "Band " + juce::String(i + 1) + " Enable", true));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            prefix + "type", "Band " + juce::String(i + 1) + " Shape",
            juce::StringArray { "Bell", "Low Shelf", "High Shelf", "Low Cut", "High Cut", "Notch", "Band Pass", "Tilt" },
            defaultTypes[i]));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            prefix + "freq", "Band " + juce::String(i + 1) + " Frequency",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.25f), defaultFreqs[i],
            juce::AudioParameterFloatAttributes().withLabel("Hz")));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            prefix + "gain", "Band " + juce::String(i + 1) + " Gain",
            juce::NormalisableRange<float>(-30.0f, 30.0f, 0.1f), defaultGains[i],
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            prefix + "q", "Band " + juce::String(i + 1) + " Q",
            juce::NormalisableRange<float>(0.1f, 30.0f, 0.01f, 0.5f), defaultQs[i]));

        // Dynamic EQ parameters
        layout.add(std::make_unique<juce::AudioParameterBool>(
            prefix + "dyn_enable", "Band " + juce::String(i + 1) + " Dynamic", false));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            prefix + "dyn_range", "Band " + juce::String(i + 1) + " Dynamic Range",
            juce::NormalisableRange<float>(-30.0f, 30.0f, 0.1f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            prefix + "dyn_thresh", "Band " + juce::String(i + 1) + " Dynamic Threshold",
            juce::NormalisableRange<float>(-60.0f, 0.0f, 0.5f), -20.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            prefix + "slope", "Band " + juce::String(i + 1) + " Slope",
            juce::StringArray { "6dB", "12dB", "18dB", "24dB", "48dB" }, 1));

        layout.add(std::make_unique<juce::AudioParameterChoice>(
            prefix + "routing", "Band " + juce::String(i + 1) + " Routing",
            juce::StringArray { "Stereo", "Mid", "Side", "Left", "Right" }, 0));
    }

    return layout;
}

bool VeviProQAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* VeviProQAudioProcessor::createEditor() { return new VeviProQAudioProcessorEditor (*this); }

void VeviProQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VeviProQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType())) {
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VeviProQAudioProcessor(); }
