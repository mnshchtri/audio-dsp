#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Amplitude-modulation tremolo. The Wave knob morphs the LFO from a smooth
// sine pulse to a choppier square-ish pulse.
class Tremolo : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        phaseIncrement = rateHz * kTwoPi / static_cast<float>(sampleRate);
        reset();
    }

    void reset() override { phase = 0.0f; }

    void setRateHz(float hz) {
        rateHz = std::clamp(hz, 0.05f, 12.0f);
        phaseIncrement = rateHz * kTwoPi / static_cast<float>(sampleRate);
    }
    void setDepth(float normalized) { depth = std::clamp(normalized, 0.0f, 1.0f); }
    void setWave(float normalized) { wave = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        float localPhase = phase;
        for (int i = 0; i < numSamples; ++i) {
            const float sine = std::sin(localPhase);
            const float squared = sine >= 0.0f ? 1.0f : -1.0f;
            const float shaped = sine * (1.0f - wave) + squared * wave;
            const float lfo = 0.5f + 0.5f * shaped;
            const float gain = 1.0f - depth * (1.0f - lfo);

            for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch)
                channelData[ch][i] *= gain;

            localPhase += phaseIncrement;
            if (localPhase >= kTwoPi) localPhase -= kTwoPi;
        }
        phase = localPhase;
    }

    const char* name() const override { return "Tremolo"; }
    const char* category() const override { return "Modulation"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Rate", 0.05f, 12.0f, rateHz, " Hz", [this](float v) { setRateHz(v); } },
            { "Depth", 0.0f, 1.0f, depth, "", [this](float v) { setDepth(v); } },
            { "Wave", 0.0f, 1.0f, wave, "", [this](float v) { setWave(v); } },
        };
    }

private:
    double sampleRate = 44100.0;
    float rateHz = 4.0f;
    float depth = 0.6f;
    float wave = 0.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
};

} // namespace audio
