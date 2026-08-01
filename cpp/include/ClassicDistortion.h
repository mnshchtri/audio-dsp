#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Harder-edged pedal distortion (Rat/DS-1 territory): a cubic soft-clip
// pushed further into hard clamping than Overdrive's tanh curve, plus a
// fixed presence bump so the squared-off waveform still cuts through.
class ClassicDistortion : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& f : toneFilters) f.setLowPass(sampleRate, toneFreq, 0.9f);
        for (auto& f : presenceFilters) f.setPeaking(sampleRate, 2200.0f, 1.0f, 4.0f);
        reset();
    }

    void reset() override {
        for (auto& f : toneFilters) f.reset();
        for (auto& f : presenceFilters) f.reset();
    }

    void setDriveDb(float db) { driveLinear = dbToLinear(db); }
    void setTone(float normalized) {
        tone = std::clamp(normalized, 0.0f, 1.0f);
        toneFreq = 1000.0f + tone * 6000.0f;
    }
    void setLevel(float normalized) { levelLinear = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            toneFilters[idx].setLowPass(sampleRate, toneFreq, 0.9f);
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                float sample = data[i] * driveLinear;
                sample = hardEdgeClip(sample);
                sample = presenceFilters[idx].process(sample);
                sample = toneFilters[idx].process(sample);
                data[i] = sample * levelLinear;
            }
        }
    }

    const char* name() const override { return "Distortion"; }
    const char* category() const override { return "Drive"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Drive", 0.0f, 40.0f, 20.0f, " dB", [this](float v) { setDriveDb(v); } },
            { "Tone", 0.0f, 1.0f, 0.5f, "", [this](float v) { setTone(v); } },
            { "Level", 0.0f, 1.0f, 0.55f, "", [this](float v) { setLevel(v); } },
        };
    }

private:
    static float hardEdgeClip(float x) {
        // Cubic soft-knee that saturates to +/-1 faster than tanh, giving a
        // squarer, buzzier edge characteristic of classic stompbox distortion.
        const float clamped = std::clamp(x, -1.5f, 1.5f);
        return clamped - (clamped * clamped * clamped) / 6.75f;
    }

    double sampleRate = 44100.0;
    float driveLinear = 10.0f;
    float tone = 0.5f;
    float toneFreq = 4000.0f;
    float levelLinear = 0.55f;
    std::array<Biquad, kMaxChannels> toneFilters;
    std::array<Biquad, kMaxChannels> presenceFilters;
};

} // namespace audio
