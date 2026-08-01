#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>

namespace audio {

// Downward noise gate: attenuates the signal when its envelope falls below
// threshold, used to clean up hiss/hum between guitar phrases.
class NoiseGate : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        for (auto& env : envelopes) {
            env.prepare(sampleRate);
            env.setAttackMs(1.0f);
            env.setReleaseMs(80.0f);
        }
        for (auto& smoother : gainSmoothers) {
            smoother.setCoefficientFromTimeMs(sampleRate, 5.0f);
        }
        reset();
    }

    void reset() override {
        for (auto& env : envelopes) env.reset();
        for (auto& smoother : gainSmoothers) smoother.reset(1.0f);
    }

    void setThresholdDb(float db) { thresholdDb = db; }
    void setAttackMs(float ms) { for (auto& env : envelopes) env.setAttackMs(ms); }
    void setReleaseMs(float ms) { for (auto& env : envelopes) env.setReleaseMs(ms); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;
        const float thresholdLinear = dbToLinear(thresholdDb);

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                const float level = envelopes[static_cast<size_t>(ch)].process(data[i]);
                const float targetGain = level >= thresholdLinear ? 1.0f : 0.0f;
                const float gain = gainSmoothers[static_cast<size_t>(ch)].process(targetGain);
                data[i] *= gain;
            }
        }
    }

    const char* name() const override { return "Noise Gate"; }
    const char* category() const override { return "Dynamics"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Thresh", -80.0f, -10.0f, -50.0f, " dB", [this](float v) { setThresholdDb(v); } },
        };
    }

private:
    double sampleRate = 44100.0;
    float thresholdDb = -50.0f;
    std::array<EnvelopeFollower, kMaxChannels> envelopes;
    std::array<OnePole, kMaxChannels> gainSmoothers;
};

} // namespace audio
