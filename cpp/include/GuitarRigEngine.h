#pragma once

#include "DspUtils.h"
#include "EffectChain.h"
#include "EffectFactory.h"
#include "Preset.h"

namespace audio {

// Top-level guitar signal-processing engine: a dynamic, user-configurable
// pedalboard (add/remove/reorder any registered effect type) with master
// input/output trim around the chain.
class GuitarRigEngine {
public:
    GuitarRigEngine() { buildDefaultChain(); }

    void prepare(double sampleRateIn) {
        sampleRate = sampleRateIn;
        chain.prepare(sampleRate);
    }

    void reset() { chain.reset(); }

    void process(float* const* channelData, int numChannels, int numSamples) {
        const float inGain = inputGainLinear;
        const float outGain = outputGainLinear;
        for (int ch = 0; ch < numChannels; ++ch) {
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) data[i] *= inGain;
        }

        chain.process(channelData, numChannels, numSamples);

        for (int ch = 0; ch < numChannels; ++ch) {
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) data[i] *= outGain;
        }
    }

    void setInputGainDb(float db) { inputGainLinear = dbToLinear(db); }
    void setOutputGainDb(float db) { outputGainLinear = dbToLinear(db); }

    size_t addModule(EffectType type, size_t atIndex) {
        auto effect = createEffect(type);
        effect->prepare(sampleRate);
        const size_t index = std::min(atIndex, chain.size());
        chain.insertEffect(index, std::move(effect));
        return index;
    }
    size_t addModule(EffectType type) { return addModule(type, chain.size()); }

    void removeModule(size_t index) { chain.removeEffect(index); }
    void moveModule(size_t fromIndex, size_t toIndex) { chain.moveEffect(fromIndex, toIndex); }

    void loadPreset(const Preset& preset) {
        chain.clear();
        for (auto& moduleSpec : preset.modules) {
            auto effect = createEffect(moduleSpec.type);
            effect->prepare(sampleRate);
            effect->setBypassed(moduleSpec.bypassed);
            for (auto& param : effect->getParameters()) {
                float value = param.currentValue;
                for (auto& override : moduleSpec.paramOverrides) {
                    if (override.first == param.label) {
                        value = override.second;
                        break;
                    }
                }
                param.setValue(value);
            }
            chain.addEffect(std::move(effect));
        }
    }

    EffectChain& getChain() { return chain; }

private:
    void buildDefaultChain() {
        addModule(EffectType::NoiseGate);
        addModule(EffectType::Compressor);
        addModule(EffectType::Overdrive);
        addModule(EffectType::Amp);
        addModule(EffectType::Cabinet);
        addModule(EffectType::Chorus);
        addModule(EffectType::Delay);
        addModule(EffectType::Reverb);

        chain.getEffect(2)->setBypassed(true); // Overdrive off by default
        chain.getEffect(5)->setBypassed(true); // Chorus off by default
        chain.getEffect(6)->setBypassed(true); // Delay off by default
        chain.getEffect(7)->setBypassed(true); // Reverb off by default
    }

    EffectChain chain;
    double sampleRate = 44100.0;
    float inputGainLinear = 1.0f;
    float outputGainLinear = 1.0f;
};

} // namespace audio
