#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>

namespace audio {

// EQ-curve approximation of a guitar speaker cabinet: high-pass to remove
// sub-bass the speaker can't reproduce, a resonant low-mid bump, a presence
// dip, and a low-pass to roll off harsh highs. Not a convolution/IR cab sim
// (no licensed impulse responses are bundled), but captures the shape.
class CabinetSim : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& f : highPass) f.setHighPass(sampleRate, 90.0f, 0.707f);
        updateBrightness();
        reset();
    }

    void reset() override {
        for (auto& f : highPass) f.reset();
        for (auto& f : resonance) f.reset();
        for (auto& f : presenceDip) f.reset();
        for (auto& f : lowPass) f.reset();
    }

    void setBrightness(float normalized) {
        brightness = std::clamp(normalized, 0.0f, 1.0f);
        updateBrightness();
    }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            float* data = channelData[ch];
            const size_t idx = static_cast<size_t>(ch);
            for (int i = 0; i < numSamples; ++i) {
                float sample = data[i];
                sample = highPass[idx].process(sample);
                sample = resonance[idx].process(sample);
                sample = presenceDip[idx].process(sample);
                sample = lowPass[idx].process(sample);
                data[i] = sample;
            }
        }
    }

    const char* name() const override { return "Cabinet"; }
    const char* category() const override { return "Amp"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Bright", 0.0f, 1.0f, brightness, "", [this](float v) { setBrightness(v); } },
        };
    }

private:
    void updateBrightness() {
        const float lowPassFreq = 3200.0f + brightness * 2500.0f;
        for (auto& f : resonance) f.setPeaking(sampleRate, 120.0f, 1.2f, 4.0f);
        for (auto& f : presenceDip) f.setPeaking(sampleRate, 1800.0f, 1.0f, -3.0f);
        for (auto& f : lowPass) f.setLowPass(sampleRate, lowPassFreq, 0.707f);
    }

    double sampleRate = 44100.0;
    float brightness = 0.5f;
    std::array<Biquad, kMaxChannels> highPass;
    std::array<Biquad, kMaxChannels> resonance;
    std::array<Biquad, kMaxChannels> presenceDip;
    std::array<Biquad, kMaxChannels> lowPass;
};

} // namespace audio
