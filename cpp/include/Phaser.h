#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

namespace detail {
// First-order allpass tuned by cutoff frequency, the building block of a
// classic 4-stage phase-shifter.
class FreqAllpass {
public:
    void setFrequency(double sampleRate, float freq) {
        const float t = std::tan(kPi * freq / static_cast<float>(sampleRate));
        coeff = (t - 1.0f) / (t + 1.0f);
    }
    float process(float x) {
        const float y = coeff * x + x1 - coeff * y1;
        x1 = x;
        y1 = y;
        return y;
    }
    void reset() { x1 = y1 = 0.0f; }

private:
    float coeff = 0.0f;
    float x1 = 0.0f, y1 = 0.0f;
};
} // namespace detail

// 4-stage allpass phaser with an LFO sweeping the notch frequencies, plus
// feedback for a deeper, more resonant sweep.
class Phaser : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        phaseIncrement = rateHz * kTwoPi / static_cast<float>(sampleRate);
        reset();
    }

    void reset() override {
        for (auto& channelStages : stages)
            for (auto& stage : channelStages) stage.reset();
        lfoPhases[0] = 0.0f;
        lfoPhases[1] = kPi / 2.0f;
        for (auto& fb : feedbackState) fb = 0.0f;
    }

    void setRateHz(float hz) {
        rateHz = std::clamp(hz, 0.02f, 8.0f);
        phaseIncrement = rateHz * kTwoPi / static_cast<float>(sampleRate);
    }
    void setDepth(float normalized) { depth = std::clamp(normalized, 0.0f, 1.0f); }
    void setFeedback(float normalized) { feedback = std::clamp(normalized, 0.0f, 0.9f); }
    void setMix(float normalized) { mix = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            float* data = channelData[ch];
            float phase = lfoPhases[idx];
            for (int i = 0; i < numSamples; ++i) {
                const float lfo = 0.5f + 0.5f * std::sin(phase);
                const float freq = kMinFreq + lfo * depth * (kMaxFreq - kMinFreq);

                float sample = data[i] + feedbackState[idx] * feedback;
                for (auto& stage : stages[idx]) {
                    stage.setFrequency(sampleRate, freq);
                    sample = stage.process(sample);
                }
                feedbackState[idx] = sample;
                data[i] = data[i] * (1.0f - mix) + sample * mix;

                phase += phaseIncrement;
                if (phase >= kTwoPi) phase -= kTwoPi;
            }
            lfoPhases[idx] = phase;
        }
    }

    const char* name() const override { return "Phaser"; }
    const char* category() const override { return "Modulation"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Rate", 0.05f, 5.0f, 0.5f, " Hz", [this](float v) { setRateHz(v); } },
            { "Depth", 0.0f, 1.0f, 0.7f, "", [this](float v) { setDepth(v); } },
            { "F.back", 0.0f, 0.9f, 0.4f, "", [this](float v) { setFeedback(v); } },
            { "Mix", 0.0f, 1.0f, 0.5f, "", [this](float v) { setMix(v); } },
        };
    }

private:
    static constexpr float kMinFreq = 200.0f;
    static constexpr float kMaxFreq = 2000.0f;
    static constexpr int kNumStages = 4;

    double sampleRate = 44100.0;
    float rateHz = 0.5f;
    float depth = 0.7f;
    float feedback = 0.4f;
    float mix = 0.5f;
    float phaseIncrement = 0.0f;
    std::array<float, kMaxChannels> lfoPhases{};
    std::array<float, kMaxChannels> feedbackState{};
    std::array<std::array<detail::FreqAllpass, kNumStages>, kMaxChannels> stages;
};

} // namespace audio
