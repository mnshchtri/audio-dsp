#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Modulated-delay chorus. Each channel gets its own LFO phase offset so a
// mono-duplicated input still comes out stereo-widened.
class Chorus : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& line : delayLines) line.prepare(sampleRate, kMaxDepthMs + kBaseDelayMs + 2.0f);
        phaseIncrement = rateHz * kTwoPi / static_cast<float>(sampleRate);
        reset();
    }

    void reset() override {
        for (auto& line : delayLines) line.reset();
        lfoPhases[0] = 0.0f;
        lfoPhases[1] = kPi / 2.0f; // quadrature offset for stereo width
    }

    void setRateHz(float hz) {
        rateHz = std::clamp(hz, 0.02f, 8.0f);
        phaseIncrement = rateHz * kTwoPi / static_cast<float>(sampleRate);
    }
    void setDepth(float normalized) { depthMs = std::clamp(normalized, 0.0f, 1.0f) * kMaxDepthMs; }
    void setMix(float normalized) { mix = std::clamp(normalized, 0.0f, 1.0f); }
    void setFeedback(float normalized) { feedback = std::clamp(normalized, 0.0f, 0.9f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            float* data = channelData[ch];
            float phase = lfoPhases[idx];
            for (int i = 0; i < numSamples; ++i) {
                const float lfo = 0.5f + 0.5f * std::sin(phase);
                const float delayMs = kBaseDelayMs + lfo * depthMs;
                const float delayed = delayLines[idx].readMs(delayMs);
                delayLines[idx].push(data[i] + delayed * feedback);
                data[i] = data[i] * (1.0f - mix) + delayed * mix;

                phase += phaseIncrement;
                if (phase >= kTwoPi) phase -= kTwoPi;
            }
            lfoPhases[idx] = phase;
        }
    }

    const char* name() const override { return "Chorus"; }
    const char* category() const override { return "Modulation"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Rate", 0.05f, 5.0f, rateHz, " Hz", [this](float v) { setRateHz(v); } },
            { "Depth", 0.0f, 1.0f, depthMs / kMaxDepthMs, "", [this](float v) { setDepth(v); } },
            { "Mix", 0.0f, 1.0f, mix, "", [this](float v) { setMix(v); } },
            { "F.back", 0.0f, 0.9f, feedback, "", [this](float v) { setFeedback(v); } },
        };
    }

private:
    static constexpr float kBaseDelayMs = 7.0f;
    static constexpr float kMaxDepthMs = 6.0f;

    double sampleRate = 44100.0;
    float rateHz = 0.8f;
    float depthMs = 3.0f;
    float mix = 0.35f;
    float feedback = 0.1f;
    float phaseIncrement = 0.0f;
    std::array<float, kMaxChannels> lfoPhases{};
    std::array<DelayLine, kMaxChannels> delayLines;
};

} // namespace audio
