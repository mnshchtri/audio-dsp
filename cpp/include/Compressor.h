#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>

namespace audio {

// Feedforward dB-domain compressor with soft-knee-free ratio curve and makeup gain.
class Compressor : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& env : envelopes) {
            env.prepare(sampleRate);
            env.setAttackMs(attackMs);
            env.setReleaseMs(releaseMs);
        }
        reset();
    }

    void reset() override {
        for (auto& env : envelopes) env.reset();
    }

    void setThresholdDb(float db) { thresholdDb = db; }
    void setRatio(float r) { ratio = std::max(1.0f, r); }
    void setAttackMs(float ms) { attackMs = ms; for (auto& env : envelopes) env.setAttackMs(ms); }
    void setReleaseMs(float ms) { releaseMs = ms; for (auto& env : envelopes) env.setReleaseMs(ms); }
    void setMakeupDb(float db) { makeupLinear = dbToLinear(db); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                const float levelDb = linearToDb(envelopes[static_cast<size_t>(ch)].process(data[i]));
                float gainReductionDb = 0.0f;
                if (levelDb > thresholdDb) {
                    gainReductionDb = (levelDb - thresholdDb) * (1.0f - 1.0f / ratio);
                }
                const float gain = dbToLinear(-gainReductionDb) * makeupLinear;
                data[i] *= gain;
            }
        }
    }

    const char* name() const override { return "Compressor"; }
    const char* category() const override { return "Dynamics"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Thresh", -40.0f, 0.0f, -18.0f, " dB", [this](float v) { setThresholdDb(v); } },
            { "Ratio", 1.0f, 20.0f, 4.0f, ":1", [this](float v) { setRatio(v); } },
            { "Attack", 0.5f, 50.0f, 8.0f, " ms", [this](float v) { setAttackMs(v); } },
            { "Release", 20.0f, 500.0f, 120.0f, " ms", [this](float v) { setReleaseMs(v); } },
            { "Makeup", 0.0f, 24.0f, 3.0f, " dB", [this](float v) { setMakeupDb(v); } },
        };
    }

private:
    double sampleRate = 44100.0;
    float thresholdDb = -18.0f;
    float ratio = 4.0f;
    float attackMs = 8.0f;
    float releaseMs = 120.0f;
    float makeupLinear = 1.0f;
    std::array<EnvelopeFollower, kMaxChannels> envelopes;
};

} // namespace audio
