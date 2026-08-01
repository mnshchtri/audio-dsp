#pragma once

#include "Preset.h"
#include <vector>

namespace audio {

// Fan tone-matches built from publicly published gear/rig interviews and
// magazine rundowns (see the credits in each description) - approximations
// by ear, not official or licensed artist presets, and not affiliated with
// or endorsed by the players named.
inline const std::vector<Preset>& getPresetLibrary() {
    static const std::vector<Preset> presets = {
        Preset{
            "John Mayer - Clean Blues",
            "Two-Rock-style clean headroom with a Klon-style always-on boost "
            "and a TS808-style lead kick, per Mayer's widely-published rig "
            "rundowns. Touch and phrasing carry the tone, not gain.",
            {
                { EffectType::NoiseGate, false, { { "Thresh", -60.0f } } },
                { EffectType::Compressor, false,
                  { { "Thresh", -22.0f }, { "Ratio", 2.5f }, { "Attack", 12.0f }, { "Release", 150.0f }, { "Makeup", 2.0f } } },
                { EffectType::Boost, false, { { "Gain", 4.0f }, { "Tilt", 0.65f }, { "Level", 0.85f } } },
                { EffectType::TubeScreamer, true, { { "Drive", 0.35f }, { "Tone", 0.4f }, { "Level", 0.6f } } },
                { EffectType::Amp, false, { { "Gain", 2.0f }, { "Bass", 0.55f }, { "Mid", 0.5f }, { "Treble", 0.6f }, { "Level", 0.65f } } },
                { EffectType::Cabinet, false, { { "Bright", 0.6f } } },
                { EffectType::Delay, false, { { "Time", 320.0f }, { "F.back", 0.22f }, { "Mix", 0.18f }, { "Damp", 5500.0f } } },
                { EffectType::Reverb, false, { { "Size", 0.35f }, { "Damp", 0.4f }, { "Mix", 0.16f } } },
            },
        },
        Preset{
            "Modern Metal Rhythm",
            "Tight gated high-gain rhythm tone in the vein of modern "
            "djent/metal players: scooped mids, clipped low end, minimal "
            "ambience so palm mutes stay percussive.",
            {
                { EffectType::NoiseGate, false, { { "Thresh", -38.0f } } },
                { EffectType::Distortion, false, { { "Drive", 32.0f }, { "Tone", 0.45f }, { "Level", 0.6f } } },
                { EffectType::Amp, false, { { "Gain", 18.0f }, { "Bass", 0.65f }, { "Mid", 0.25f }, { "Treble", 0.6f }, { "Level", 0.55f } } },
                { EffectType::Cabinet, false, { { "Bright", 0.4f } } },
                { EffectType::Reverb, false, { { "Size", 0.2f }, { "Damp", 0.6f }, { "Mix", 0.08f } } },
            },
        },
        Preset{
            "Ambient Wash",
            "Gilmour/post-rock inspired wall of sound: creamy overdrive "
            "into a lush chorus, long modulated delay, and a cavernous "
            "reverb tail.",
            {
                { EffectType::Compressor, false,
                  { { "Thresh", -24.0f }, { "Ratio", 3.0f }, { "Attack", 20.0f }, { "Release", 200.0f }, { "Makeup", 4.0f } } },
                { EffectType::Overdrive, false, { { "Drive", 10.0f }, { "Tone", 0.55f }, { "Level", 0.7f } } },
                { EffectType::Amp, false, { { "Gain", 4.0f }, { "Bass", 0.55f }, { "Mid", 0.45f }, { "Treble", 0.55f }, { "Level", 0.6f } } },
                { EffectType::Cabinet, false, { { "Bright", 0.5f } } },
                { EffectType::Chorus, false, { { "Rate", 0.6f }, { "Depth", 0.7f }, { "Mix", 0.5f }, { "F.back", 0.15f } } },
                { EffectType::Delay, false, { { "Time", 550.0f }, { "F.back", 0.5f }, { "Mix", 0.4f }, { "Damp", 4000.0f } } },
                { EffectType::Reverb, false, { { "Size", 0.85f }, { "Damp", 0.35f }, { "Mix", 0.55f } } },
            },
        },
        Preset{
            "Classic Rock Crunch",
            "Bar-chord friendly AC/DC-and-friends crunch: a mid-gain "
            "overdrive pushing a cranked-sounding amp, plate reverb, "
            "nothing else in the way.",
            {
                { EffectType::NoiseGate, false, { { "Thresh", -45.0f } } },
                { EffectType::Overdrive, false, { { "Drive", 16.0f }, { "Tone", 0.5f }, { "Level", 0.75f } } },
                { EffectType::Amp, false, { { "Gain", 10.0f }, { "Bass", 0.55f }, { "Mid", 0.55f }, { "Treble", 0.55f }, { "Level", 0.6f } } },
                { EffectType::Cabinet, false, { { "Bright", 0.5f } } },
                { EffectType::Reverb, false, { { "Size", 0.4f }, { "Damp", 0.35f }, { "Mix", 0.2f } } },
            },
        },
        Preset{
            "Funk Clean",
            "Bright, percussive rhythm tone in the style of modern funk "
            "players: fast, punchy compression up front and a squeaky-clean "
            "amp so every muted stab cuts through.",
            {
                { EffectType::Compressor, false,
                  { { "Thresh", -18.0f }, { "Ratio", 6.0f }, { "Attack", 3.0f }, { "Release", 80.0f }, { "Makeup", 5.0f } } },
                { EffectType::Amp, false, { { "Gain", -2.0f }, { "Bass", 0.5f }, { "Mid", 0.55f }, { "Treble", 0.65f }, { "Level", 0.7f } } },
                { EffectType::Cabinet, false, { { "Bright", 0.7f } } },
                { EffectType::Chorus, false, { { "Rate", 1.2f }, { "Depth", 0.25f }, { "Mix", 0.15f }, { "F.back", 0.05f } } },
            },
        },
    };
    return presets;
}

} // namespace audio
