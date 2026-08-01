#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>

namespace audio {

// Feedback delay line with a damping filter in the feedback path so repeats
// darken over time, like an analog/tape delay.
class Delay : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& line : delayLines) line.prepare(sampleRate, kMaxDelayMs + 2.0f);
        for (auto& f : dampingFilters) f.setLowPass(sampleRate, dampingFreq, 0.707f);
        reset();
    }

    void reset() override {
        for (auto& line : delayLines) line.reset();
        for (auto& f : dampingFilters) f.reset();
    }

    void setTimeMs(float ms) { timeMs = std::clamp(ms, 1.0f, kMaxDelayMs); }
    void setFeedback(float normalized) { feedback = std::clamp(normalized, 0.0f, 0.95f); }
    void setMix(float normalized) { mix = std::clamp(normalized, 0.0f, 1.0f); }
    void setDampingHz(float hz) {
        dampingFreq = std::clamp(hz, 500.0f, 18000.0f);
        for (auto& f : dampingFilters) f.setLowPass(sampleRate, dampingFreq, 0.707f);
    }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                const float delayed = delayLines[idx].readMs(timeMs);
                const float damped = dampingFilters[idx].process(delayed);
                delayLines[idx].push(data[i] + damped * feedback);
                data[i] = data[i] * (1.0f - mix) + delayed * mix;
            }
        }
    }

    const char* name() const override { return "Delay"; }
    const char* category() const override { return "Time"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Time", 20.0f, 1200.0f, 350.0f, " ms", [this](float v) { setTimeMs(v); } },
            { "F.back", 0.0f, 0.95f, 0.35f, "", [this](float v) { setFeedback(v); } },
            { "Mix", 0.0f, 1.0f, 0.3f, "", [this](float v) { setMix(v); } },
            { "Damp", 500.0f, 18000.0f, 6000.0f, " Hz", [this](float v) { setDampingHz(v); } },
        };
    }

private:
    static constexpr float kMaxDelayMs = 1500.0f;

    double sampleRate = 44100.0;
    float timeMs = 350.0f;
    float feedback = 0.35f;
    float mix = 0.3f;
    float dampingFreq = 6000.0f;
    std::array<DelayLine, kMaxChannels> delayLines;
    std::array<Biquad, kMaxChannels> dampingFilters;
};

} // namespace audio
