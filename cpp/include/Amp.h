#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Preamp saturation stage (asymmetric soft clip for tube-like warmth) followed
// by a classic bass/mid/treble tone stack.
class Amp : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        updateToneStack();
        reset();
    }

    void reset() override {
        for (auto& stage : bassFilters) stage.reset();
        for (auto& stage : midFilters) stage.reset();
        for (auto& stage : trebleFilters) stage.reset();
    }

    void setGainDb(float db) { gainLinear = dbToLinear(db); }
    void setBass(float normalized) { bassDb = (std::clamp(normalized, 0.0f, 1.0f) - 0.5f) * 24.0f; updateToneStack(); }
    void setMid(float normalized) { midDb = (std::clamp(normalized, 0.0f, 1.0f) - 0.5f) * 24.0f; updateToneStack(); }
    void setTreble(float normalized) { trebleDb = (std::clamp(normalized, 0.0f, 1.0f) - 0.5f) * 24.0f; updateToneStack(); }
    void setLevel(float normalized) { levelLinear = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                const float driven = data[i] * gainLinear;
                const float saturated = saturate(driven);
                float sample = saturated;
                sample = bassFilters[static_cast<size_t>(ch)].process(sample);
                sample = midFilters[static_cast<size_t>(ch)].process(sample);
                sample = trebleFilters[static_cast<size_t>(ch)].process(sample);
                data[i] = sample * levelLinear;
            }
        }
    }

    const char* name() const override { return "Amp"; }
    const char* category() const override { return "Amp"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Gain", -12.0f, 24.0f, 6.0f, " dB", [this](float v) { setGainDb(v); } },
            { "Bass", 0.0f, 1.0f, 0.5f, "", [this](float v) { setBass(v); } },
            { "Mid", 0.0f, 1.0f, 0.5f, "", [this](float v) { setMid(v); } },
            { "Treble", 0.0f, 1.0f, 0.5f, "", [this](float v) { setTreble(v); } },
            { "Level", 0.0f, 1.0f, 0.6f, "", [this](float v) { setLevel(v); } },
        };
    }

private:
    static float saturate(float x) {
        // Asymmetric soft clip: slightly different curve for positive/negative
        // half-cycles, approximating single-ended tube preamp behavior.
        if (x >= 0.0f)
            return std::tanh(x);
        return std::tanh(x * 1.15f) / 1.15f;
    }

    void updateToneStack() {
        for (auto& stage : bassFilters) stage.setLowShelf(sampleRate, 150.0f, 1.0f, bassDb);
        for (auto& stage : midFilters) stage.setPeaking(sampleRate, 800.0f, 0.7f, midDb);
        for (auto& stage : trebleFilters) stage.setHighShelf(sampleRate, 3000.0f, 1.0f, trebleDb);
    }

    double sampleRate = 44100.0;
    float gainLinear = 3.0f;
    float bassDb = 0.0f;
    float midDb = 0.0f;
    float trebleDb = 0.0f;
    float levelLinear = 0.6f;
    std::array<Biquad, kMaxChannels> bassFilters;
    std::array<Biquad, kMaxChannels> midFilters;
    std::array<Biquad, kMaxChannels> trebleFilters;
};

} // namespace audio
