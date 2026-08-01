#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Drive/tone/level overdrive-to-distortion stage. Blends a smooth tanh soft
// clip with a harder clip as drive increases, followed by a tone tilt filter.
class Overdrive : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& filter : toneFilters) filter.setLowPass(sampleRate, 4000.0f, 0.707f);
        reset();
    }

    void reset() override {
        for (auto& filter : toneFilters) filter.reset();
    }

    void setDriveDb(float db) { driveDb = db; driveLinear = dbToLinear(db); }
    void setTone(float normalized) {
        tone = std::clamp(normalized, 0.0f, 1.0f);
        toneFreq = 800.0f + tone * 6000.0f;
    }
    void setLevel(float normalized) { levelLinear = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            toneFilters[static_cast<size_t>(ch)].setLowPass(sampleRate, toneFreq, 0.707f);
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                const float driven = data[i] * driveLinear;
                const float softClipped = std::tanh(driven);
                const float hardClipped = std::clamp(driven, -1.0f, 1.0f);
                const float blend = std::clamp((driveLinear - 1.0f) / 20.0f, 0.0f, 1.0f);
                const float shaped = softClipped * (1.0f - blend) + hardClipped * blend;
                const float toned = toneFilters[static_cast<size_t>(ch)].process(shaped);
                data[i] = toned * levelLinear;
            }
        }
    }

    const char* name() const override { return "Overdrive"; }
    const char* category() const override { return "Drive"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Drive", 0.0f, 36.0f, driveDb, " dB", [this](float v) { setDriveDb(v); } },
            { "Tone", 0.0f, 1.0f, tone, "", [this](float v) { setTone(v); } },
            { "Level", 0.0f, 1.0f, levelLinear, "", [this](float v) { setLevel(v); } },
        };
    }

private:
    double sampleRate = 44100.0;
    float driveDb = 6.0f;
    float driveLinear = dbToLinear(6.0f);
    float tone = 0.5f;
    float toneFreq = 4000.0f;
    float levelLinear = 0.7f;
    std::array<Biquad, kMaxChannels> toneFilters;
};

} // namespace audio
