#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Classic op-amp/diode-clipper overdrive: a fixed midrange hump ahead of the
// clipping stage (the defining "TS" character), a soft diode-style clip, and
// a single-pole treble-cut tone control.
class TubeScreamer : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& f : midBoost) f.setPeaking(sampleRate, 720.0f, 0.9f, 8.0f);
        for (auto& f : toneFilters) f.setLowPass(sampleRate, toneFreq, 0.707f);
        reset();
    }

    void reset() override {
        for (auto& f : midBoost) f.reset();
        for (auto& f : toneFilters) f.reset();
    }

    void setDrive(float normalized) { driveLinear = 1.0f + std::clamp(normalized, 0.0f, 1.0f) * 40.0f; }
    void setTone(float normalized) {
        tone = std::clamp(normalized, 0.0f, 1.0f);
        toneFreq = 600.0f + tone * 5000.0f;
    }
    void setLevel(float normalized) { levelLinear = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            toneFilters[idx].setLowPass(sampleRate, toneFreq, 0.707f);
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                float sample = midBoost[idx].process(data[i]);
                sample *= driveLinear;
                sample = diodeClip(sample);
                sample = toneFilters[idx].process(sample);
                data[i] = sample * levelLinear;
            }
        }
    }

    const char* name() const override { return "Tube Screamer"; }
    const char* category() const override { return "Drive"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Drive", 0.0f, 1.0f, 0.4f, "", [this](float v) { setDrive(v); } },
            { "Tone", 0.0f, 1.0f, 0.5f, "", [this](float v) { setTone(v); } },
            { "Level", 0.0f, 1.0f, 0.6f, "", [this](float v) { setLevel(v); } },
        };
    }

private:
    static float diodeClip(float x) {
        const float sign = x >= 0.0f ? 1.0f : -1.0f;
        return sign * (1.0f - std::exp(-std::abs(x)));
    }

    double sampleRate = 44100.0;
    float driveLinear = 17.0f;
    float tone = 0.5f;
    float toneFreq = 3000.0f;
    float levelLinear = 0.6f;
    std::array<Biquad, kMaxChannels> midBoost;
    std::array<Biquad, kMaxChannels> toneFilters;
};

} // namespace audio
