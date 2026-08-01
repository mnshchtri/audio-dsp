#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>

namespace audio {

// Clean boost: gain plus a tilt EQ (single knob sweeps from a dark low-boost
// voicing to a bright high-boost voicing), the simplest pedal in the rack.
class Boost : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        updateTilt();
        reset();
    }

    void reset() override {
        for (auto& f : lowShelf) f.reset();
        for (auto& f : highShelf) f.reset();
    }

    void setGainDb(float db) { gainDb = db; gainLinear = dbToLinear(db); }
    void setTilt(float normalized) { tilt = std::clamp(normalized, 0.0f, 1.0f); updateTilt(); }
    void setLevel(float normalized) { levelLinear = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                float sample = data[i] * gainLinear;
                sample = lowShelf[idx].process(sample);
                sample = highShelf[idx].process(sample);
                data[i] = sample * levelLinear;
            }
        }
    }

    const char* name() const override { return "Boost"; }
    const char* category() const override { return "Drive"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Gain", 0.0f, 20.0f, gainDb, " dB", [this](float v) { setGainDb(v); } },
            { "Tilt", 0.0f, 1.0f, tilt, "", [this](float v) { setTilt(v); } },
            { "Level", 0.0f, 1.0f, levelLinear, "", [this](float v) { setLevel(v); } },
        };
    }

private:
    void updateTilt() {
        const float shelfDb = (tilt - 0.5f) * 12.0f;
        for (auto& f : lowShelf) f.setLowShelf(sampleRate, 200.0f, 1.0f, -shelfDb);
        for (auto& f : highShelf) f.setHighShelf(sampleRate, 2000.0f, 1.0f, shelfDb);
    }

    double sampleRate = 44100.0;
    float gainDb = 8.0f;
    float gainLinear = dbToLinear(8.0f);
    float tilt = 0.5f;
    float levelLinear = 0.8f;
    std::array<Biquad, kMaxChannels> lowShelf;
    std::array<Biquad, kMaxChannels> highShelf;
};

} // namespace audio
