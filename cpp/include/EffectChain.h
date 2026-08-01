#pragma once

#include "Effect.h"
#include <algorithm>
#include <memory>
#include <vector>

namespace audio {

// Owns an ordered, mutable list of effects and runs them in sequence, in
// place, on the same buffer. Unlike a fixed pedalboard, modules can be
// added, removed, and reordered at runtime (the effect browser + drag-to-
// reorder chain strip in the UI).
class EffectChain {
public:
    void prepare(double sampleRate) {
        preparedSampleRate = sampleRate;
        for (auto& effect : effects) effect->prepare(sampleRate);
    }

    void reset() {
        for (auto& effect : effects) effect->reset();
    }

    void process(float* const* channelData, int numChannels, int numSamples) {
        for (auto& effect : effects) effect->process(channelData, numChannels, numSamples);
    }

    void insertEffect(size_t index, std::unique_ptr<Effect> effect) {
        if (preparedSampleRate > 0.0) effect->prepare(preparedSampleRate);
        const size_t clampedIndex = std::min(index, effects.size());
        effects.insert(effects.begin() + static_cast<long>(clampedIndex), std::move(effect));
    }

    void addEffect(std::unique_ptr<Effect> effect) { insertEffect(effects.size(), std::move(effect)); }

    void removeEffect(size_t index) {
        if (index < effects.size()) effects.erase(effects.begin() + static_cast<long>(index));
    }

    void moveEffect(size_t fromIndex, size_t toIndex) {
        if (fromIndex >= effects.size() || toIndex >= effects.size() || fromIndex == toIndex) return;
        auto effect = std::move(effects[fromIndex]);
        effects.erase(effects.begin() + static_cast<long>(fromIndex));
        effects.insert(effects.begin() + static_cast<long>(toIndex), std::move(effect));
    }

    Effect* getEffect(size_t index) const {
        return index < effects.size() ? effects[index].get() : nullptr;
    }

    size_t size() const { return effects.size(); }

private:
    double preparedSampleRate = 0.0;
    std::vector<std::unique_ptr<Effect>> effects;
};

} // namespace audio
