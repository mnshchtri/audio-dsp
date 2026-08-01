#pragma once

#include "Amp.h"
#include "Boost.h"
#include "CabinetSim.h"
#include "ClassicDistortion.h"
#include "Chorus.h"
#include "Compressor.h"
#include "Delay.h"
#include "Effect.h"
#include "Fuzz.h"
#include "NoiseGate.h"
#include "Overdrive.h"
#include "Phaser.h"
#include "Reverb.h"
#include "Tremolo.h"
#include "TubeScreamer.h"
#include <memory>
#include <vector>

namespace audio {

// Every effect type the UI's effect browser can offer, independent of what's
// currently in the chain.
enum class EffectType {
    NoiseGate,
    Compressor,
    Boost,
    Overdrive,
    TubeScreamer,
    Distortion,
    Fuzz,
    Amp,
    Cabinet,
    Chorus,
    Phaser,
    Tremolo,
    Delay,
    Reverb,
};

struct EffectTypeInfo {
    EffectType type;
    const char* name;
    const char* category;
};

inline const std::vector<EffectTypeInfo>& getEffectTypeRegistry() {
    static const std::vector<EffectTypeInfo> registry = {
        { EffectType::NoiseGate, "Noise Gate", "Dynamics" },
        { EffectType::Compressor, "Compressor", "Dynamics" },
        { EffectType::Boost, "Boost", "Drive" },
        { EffectType::Overdrive, "Overdrive", "Drive" },
        { EffectType::TubeScreamer, "Tube Screamer", "Drive" },
        { EffectType::Distortion, "Distortion", "Drive" },
        { EffectType::Fuzz, "Fuzz", "Drive" },
        { EffectType::Amp, "Amp", "Amp" },
        { EffectType::Cabinet, "Cabinet", "Amp" },
        { EffectType::Chorus, "Chorus", "Modulation" },
        { EffectType::Phaser, "Phaser", "Modulation" },
        { EffectType::Tremolo, "Tremolo", "Modulation" },
        { EffectType::Delay, "Delay", "Time" },
        { EffectType::Reverb, "Reverb", "Reverb" },
    };
    return registry;
}

inline std::unique_ptr<Effect> createEffect(EffectType type) {
    switch (type) {
        case EffectType::NoiseGate: return std::make_unique<NoiseGate>();
        case EffectType::Compressor: return std::make_unique<Compressor>();
        case EffectType::Boost: return std::make_unique<Boost>();
        case EffectType::Overdrive: return std::make_unique<Overdrive>();
        case EffectType::TubeScreamer: return std::make_unique<TubeScreamer>();
        case EffectType::Distortion: return std::make_unique<ClassicDistortion>();
        case EffectType::Fuzz: return std::make_unique<Fuzz>();
        case EffectType::Amp: return std::make_unique<Amp>();
        case EffectType::Cabinet: return std::make_unique<CabinetSim>();
        case EffectType::Chorus: return std::make_unique<Chorus>();
        case EffectType::Phaser: return std::make_unique<Phaser>();
        case EffectType::Tremolo: return std::make_unique<Tremolo>();
        case EffectType::Delay: return std::make_unique<Delay>();
        case EffectType::Reverb: return std::make_unique<Reverb>();
    }
    return nullptr;
}

} // namespace audio
