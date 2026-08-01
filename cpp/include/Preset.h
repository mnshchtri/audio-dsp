#pragma once

#include "EffectFactory.h"
#include <string>
#include <utility>
#include <vector>

namespace audio {

// One pedal within a preset's chain: which effect, whether it starts
// bypassed, and any parameter values that differ from that effect's
// built-in defaults (matched against Effect::getParameters() by label).
struct PresetModule {
    EffectType type;
    bool bypassed = false;
    std::vector<std::pair<std::string, float>> paramOverrides;
};

// A full pedalboard configuration: the whole chain, in order, ready to drop
// onto the engine in one call.
struct Preset {
    std::string name;
    std::string description;
    std::vector<PresetModule> modules;
};

} // namespace audio
