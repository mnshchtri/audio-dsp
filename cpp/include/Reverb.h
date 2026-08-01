#pragma once

#include "DspUtils.h"
#include "Effect.h"
#include <array>
#include <cmath>
#include <vector>

namespace audio {

namespace detail {

// Feedback comb filter with a one-pole damping filter inside the loop
// (Schroeder-Moorer style), the basic building block of the reverb tail.
class DampedComb {
public:
    void prepare(double sampleRate, float baseDelayMs, float scale) {
        const int size = static_cast<int>(std::round(0.001 * baseDelayMs * scale * sampleRate));
        buffer.assign(static_cast<size_t>(std::max(size, 1)), 0.0f);
        index = 0;
    }

    void setFeedback(float fb) { feedback = fb; }
    void setDamping(float d) { damping = d; }
    void reset() { std::fill(buffer.begin(), buffer.end(), 0.0f); dampState = 0.0f; }

    float process(float input) {
        const float output = buffer[static_cast<size_t>(index)];
        dampState = output * (1.0f - damping) + dampState * damping;
        buffer[static_cast<size_t>(index)] = input + dampState * feedback;
        index = (index + 1) % static_cast<int>(buffer.size());
        return output;
    }

private:
    std::vector<float> buffer;
    int index = 0;
    float feedback = 0.5f;
    float damping = 0.2f;
    float dampState = 0.0f;
};

// Allpass filter used in series after the comb bank to diffuse the tail.
class Allpass {
public:
    void prepare(double sampleRate, float delayMs) {
        const int size = static_cast<int>(std::round(0.001 * delayMs * sampleRate));
        buffer.assign(static_cast<size_t>(std::max(size, 1)), 0.0f);
        index = 0;
    }

    void reset() { std::fill(buffer.begin(), buffer.end(), 0.0f); }

    float process(float input) {
        const float bufferedValue = buffer[static_cast<size_t>(index)];
        const float output = -input + bufferedValue;
        buffer[static_cast<size_t>(index)] = input + bufferedValue * kGain;
        index = (index + 1) % static_cast<int>(buffer.size());
        return output;
    }

private:
    static constexpr float kGain = 0.5f;
    std::vector<float> buffer;
    int index = 0;
};

} // namespace detail

// Schroeder/Freeverb-style reverb: four parallel damped combs summed and fed
// through two series allpass filters, per channel with a small stereo offset.
class Reverb : public Effect {
public:
    void prepare(double sampleRateIn) override {
        sampleRate = sampleRateIn;
        static constexpr std::array<float, kNumCombs> combBaseMs{ 25.3f, 26.9f, 30.8f, 32.2f };
        static constexpr std::array<float, kNumAllpass> allpassBaseMs{ 12.6f, 10.0f };

        for (int ch = 0; ch < kMaxChannels; ++ch) {
            const float stereoOffset = ch == 0 ? 1.0f : 1.02f;
            for (int c = 0; c < kNumCombs; ++c)
                combs[static_cast<size_t>(ch)][static_cast<size_t>(c)].prepare(sampleRate, combBaseMs[static_cast<size_t>(c)], stereoOffset);
            for (int a = 0; a < kNumAllpass; ++a)
                allpasses[static_cast<size_t>(ch)][static_cast<size_t>(a)].prepare(sampleRate, allpassBaseMs[static_cast<size_t>(a)] * stereoOffset);
        }
        updateCombParams();
        reset();
    }

    void reset() override {
        for (auto& channelCombs : combs)
            for (auto& comb : channelCombs) comb.reset();
        for (auto& channelAllpasses : allpasses)
            for (auto& ap : channelAllpasses) ap.reset();
    }

    void setRoomSize(float normalized) { roomSize = std::clamp(normalized, 0.0f, 1.0f); updateCombParams(); }
    void setDamping(float normalized) { damping = std::clamp(normalized, 0.0f, 1.0f); updateCombParams(); }
    void setMix(float normalized) { mix = std::clamp(normalized, 0.0f, 1.0f); }

    void process(float* const* channelData, int numChannels, int numSamples) override {
        if (bypassed) return;

        for (int ch = 0; ch < numChannels && ch < kMaxChannels; ++ch) {
            const size_t idx = static_cast<size_t>(ch);
            float* data = channelData[ch];
            for (int i = 0; i < numSamples; ++i) {
                const float input = data[i];
                float wet = 0.0f;
                for (auto& comb : combs[idx]) wet += comb.process(input);
                wet /= static_cast<float>(kNumCombs);
                for (auto& ap : allpasses[idx]) wet = ap.process(wet);
                data[i] = input * (1.0f - mix) + wet * mix;
            }
        }
    }

    const char* name() const override { return "Reverb"; }
    const char* category() const override { return "Reverb"; }

    std::vector<ParamInfo> getParameters() override {
        return {
            { "Size", 0.0f, 1.0f, roomSize, "", [this](float v) { setRoomSize(v); } },
            { "Damp", 0.0f, 1.0f, damping, "", [this](float v) { setDamping(v); } },
            { "Mix", 0.0f, 1.0f, mix, "", [this](float v) { setMix(v); } },
        };
    }

private:
    static constexpr int kNumCombs = 4;
    static constexpr int kNumAllpass = 2;

    void updateCombParams() {
        const float feedback = 0.7f + roomSize * 0.28f;
        for (auto& channelCombs : combs) {
            for (auto& comb : channelCombs) {
                comb.setFeedback(feedback);
                comb.setDamping(damping);
            }
        }
    }

    double sampleRate = 44100.0;
    float roomSize = 0.5f;
    float damping = 0.3f;
    float mix = 0.25f;
    std::array<std::array<detail::DampedComb, kNumCombs>, kMaxChannels> combs;
    std::array<std::array<detail::Allpass, kNumAllpass>, kMaxChannels> allpasses;
};

} // namespace audio
