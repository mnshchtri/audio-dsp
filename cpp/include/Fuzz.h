#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Silicon fuzz-face style: extreme gain into a near-square hard clip, with a
// touch of rectified signal blended in for the buzzy, gated fuzz texture,
// and an input-side noise gate since fuzz circuits hiss heavily at rest.
class Fuzz : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& env : gateEnvelopes) env.prepare(sampleRate);
        for (auto& env : gateEnvelopes) { env.setAttackMs(1.0f); env.setReleaseMs(60.0f); }
        for (auto& f : toneFilters) f.setLowPass(sampleRate, toneFreq, 0.8f);
        reset();
    }

    void reset() override {
        for (auto& env : gateEnvelopes) env.reset();
        for (auto& f : toneFilters) f.reset();
    }

    void setFuzz(float normalized) { fuzzLinear = 1.0f + std::clamp(normalized, 0.0f, 1.0f) * 120.0f; }
    void setTone(float normalized) {
        tone = std::clamp(normalized, 0.0f, 1.0f);
        toneFreq = 900.0f + tone * 6000.0f;
    }
    void setLevel(float normalized) { levelLinear = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            toneFilters[idx].setLowPass(sampleRate, toneFreq, 0.8f);
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                const float level = gateEnvelopes[idx].process(data[i]);
                const float gate = level >= 0.0015f ? 1.0f : (level / 0.0015f);

                float sample = data[i] * fuzzLinear;
                const float squared = std::clamp(sample, -1.0f, 1.0f);
                // Full-wave-rectified component folds the waveform, adding the
                // buzzy even-harmonic content characteristic of silicon fuzz.
                const float buzz = 2.0f * std::abs(squared) - 1.0f;
                sample = (squared * 0.85f + buzz * 0.15f) * gate;
                sample = toneFilters[idx].process(sample);
                data[i] = sample * levelLinear;
            }
        }
    }

    const char* name() const override { return "Fuzz"; }
    const char* category() const override { return "Drive"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Fuzz", 0.0f, 1.0f, 0.6f, "", [this](float v) { setFuzz(v); } },
            { "Tone", 0.0f, 1.0f, 0.5f, "", [this](float v) { setTone(v); } },
            { "Level", 0.0f, 1.0f, 0.5f, "", [this](float v) { setLevel(v); } },
        };
    }

private:
    double sampleRate = 44100.0;
    float fuzzLinear = 73.0f;
    float tone = 0.5f;
    float toneFreq = 3900.0f;
    float levelLinear = 0.5f;
    std::array<EnvelopeFollower, kMaxChannels> gateEnvelopes;
    std::array<Biquad, kMaxChannels> toneFilters;
};

} // namespace audio
