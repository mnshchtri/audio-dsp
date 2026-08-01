#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

namespace audio {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 2.0f * kPi;

inline float dbToLinear(float db) {
    return std::pow(10.0f, db / 20.0f);
}

inline float linearToDb(float linear) {
    return 20.0f * std::log10(std::max(linear, 1.0e-9f));
}

// One-pole smoother, used for parameter/envelope smoothing.
class OnePole {
public:
    void setCoefficientFromTimeMs(double sampleRate, float timeMs) {
        coeff = std::exp(-1.0f / (0.001f * timeMs * static_cast<float>(sampleRate)));
    }

    float process(float input) {
        state = input + coeff * (state - input);
        return state;
    }

    void reset(float value = 0.0f) { state = value; }

private:
    float coeff = 0.0f;
    float state = 0.0f;
};

// Attack/release envelope follower operating on the absolute value of the signal.
class EnvelopeFollower {
public:
    void prepare(double sampleRateIn) { sampleRate = sampleRateIn; }

    void setAttackMs(float ms) {
        attackCoeff = std::exp(-1.0f / (0.001f * std::max(ms, 0.01f) * static_cast<float>(sampleRate)));
    }

    void setReleaseMs(float ms) {
        releaseCoeff = std::exp(-1.0f / (0.001f * std::max(ms, 0.01f) * static_cast<float>(sampleRate)));
    }

    float process(float input) {
        const float rectified = std::abs(input);
        const float coeff = rectified > envelope ? attackCoeff : releaseCoeff;
        envelope = rectified + coeff * (envelope - rectified);
        return envelope;
    }

    void reset() { envelope = 0.0f; }

private:
    double sampleRate = 44100.0;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float envelope = 0.0f;
};

// RBJ biquad filter (Audio EQ Cookbook).
class Biquad {
public:
    enum class Type { LowPass, HighPass, Peaking, LowShelf, HighShelf, AllPass, BandPass };

    void setLowPass(double sampleRate, float freq, float q) {
        const double w0 = kTwoPi * freq / sampleRate;
        const double cosw0 = std::cos(w0);
        const double sinw0 = std::sin(w0);
        const double alpha = sinw0 / (2.0 * q);
        const double b0 = (1.0 - cosw0) / 2.0;
        const double b1 = 1.0 - cosw0;
        const double b2 = (1.0 - cosw0) / 2.0;
        const double a0 = 1.0 + alpha;
        const double a1 = -2.0 * cosw0;
        const double a2 = 1.0 - alpha;
        setCoeffs(b0, b1, b2, a0, a1, a2);
    }

    void setHighPass(double sampleRate, float freq, float q) {
        const double w0 = kTwoPi * freq / sampleRate;
        const double cosw0 = std::cos(w0);
        const double sinw0 = std::sin(w0);
        const double alpha = sinw0 / (2.0 * q);
        const double b0 = (1.0 + cosw0) / 2.0;
        const double b1 = -(1.0 + cosw0);
        const double b2 = (1.0 + cosw0) / 2.0;
        const double a0 = 1.0 + alpha;
        const double a1 = -2.0 * cosw0;
        const double a2 = 1.0 - alpha;
        setCoeffs(b0, b1, b2, a0, a1, a2);
    }

    void setBandPass(double sampleRate, float freq, float q) {
        const double w0 = kTwoPi * freq / sampleRate;
        const double cosw0 = std::cos(w0);
        const double sinw0 = std::sin(w0);
        const double alpha = sinw0 / (2.0 * q);
        const double b0 = alpha;
        const double b1 = 0.0;
        const double b2 = -alpha;
        const double a0 = 1.0 + alpha;
        const double a1 = -2.0 * cosw0;
        const double a2 = 1.0 - alpha;
        setCoeffs(b0, b1, b2, a0, a1, a2);
    }

    void setPeaking(double sampleRate, float freq, float q, float gainDb) {
        const double A = std::pow(10.0, gainDb / 40.0);
        const double w0 = kTwoPi * freq / sampleRate;
        const double cosw0 = std::cos(w0);
        const double sinw0 = std::sin(w0);
        const double alpha = sinw0 / (2.0 * q);
        const double b0 = 1.0 + alpha * A;
        const double b1 = -2.0 * cosw0;
        const double b2 = 1.0 - alpha * A;
        const double a0 = 1.0 + alpha / A;
        const double a1 = -2.0 * cosw0;
        const double a2 = 1.0 - alpha / A;
        setCoeffs(b0, b1, b2, a0, a1, a2);
    }

    void setLowShelf(double sampleRate, float freq, float slope, float gainDb) {
        const double A = std::pow(10.0, gainDb / 40.0);
        const double w0 = kTwoPi * freq / sampleRate;
        const double cosw0 = std::cos(w0);
        const double sinw0 = std::sin(w0);
        const double alpha = sinw0 / 2.0 * std::sqrt((A + 1.0 / A) * (1.0 / slope - 1.0) + 2.0);
        const double twoSqrtAAlpha = 2.0 * std::sqrt(A) * alpha;
        const double b0 = A * ((A + 1.0) - (A - 1.0) * cosw0 + twoSqrtAAlpha);
        const double b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosw0);
        const double b2 = A * ((A + 1.0) - (A - 1.0) * cosw0 - twoSqrtAAlpha);
        const double a0 = (A + 1.0) + (A - 1.0) * cosw0 + twoSqrtAAlpha;
        const double a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosw0);
        const double a2 = (A + 1.0) + (A - 1.0) * cosw0 - twoSqrtAAlpha;
        setCoeffs(b0, b1, b2, a0, a1, a2);
    }

    void setHighShelf(double sampleRate, float freq, float slope, float gainDb) {
        const double A = std::pow(10.0, gainDb / 40.0);
        const double w0 = kTwoPi * freq / sampleRate;
        const double cosw0 = std::cos(w0);
        const double sinw0 = std::sin(w0);
        const double alpha = sinw0 / 2.0 * std::sqrt((A + 1.0 / A) * (1.0 / slope - 1.0) + 2.0);
        const double twoSqrtAAlpha = 2.0 * std::sqrt(A) * alpha;
        const double b0 = A * ((A + 1.0) + (A - 1.0) * cosw0 + twoSqrtAAlpha);
        const double b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosw0);
        const double b2 = A * ((A + 1.0) + (A - 1.0) * cosw0 - twoSqrtAAlpha);
        const double a0 = (A + 1.0) - (A - 1.0) * cosw0 + twoSqrtAAlpha;
        const double a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cosw0);
        const double a2 = (A + 1.0) - (A - 1.0) * cosw0 - twoSqrtAAlpha;
        setCoeffs(b0, b1, b2, a0, a1, a2);
    }

    float process(float input) {
        const float output = coeffB0 * input + z1;
        z1 = coeffB1 * input - coeffA1 * output + z2;
        z2 = coeffB2 * input - coeffA2 * output;
        return output;
    }

    void reset() { z1 = z2 = 0.0f; }

private:
    void setCoeffs(double rb0, double rb1, double rb2, double a0, double ra1, double ra2) {
        coeffB0 = static_cast<float>(rb0 / a0);
        coeffB1 = static_cast<float>(rb1 / a0);
        coeffB2 = static_cast<float>(rb2 / a0);
        coeffA1 = static_cast<float>(ra1 / a0);
        coeffA2 = static_cast<float>(ra2 / a0);
    }

    float coeffB0 = 1.0f, coeffB1 = 0.0f, coeffB2 = 0.0f, coeffA1 = 0.0f, coeffA2 = 0.0f;
    float z1 = 0.0f, z2 = 0.0f;
};

// Fractional-delay circular buffer line with linear interpolation.
class DelayLine {
public:
    void prepare(double sampleRateIn, float maxDelayMs) {
        sampleRate = sampleRateIn;
        const int capacity = static_cast<int>(std::ceil(0.001 * maxDelayMs * sampleRate)) + 4;
        buffer.assign(static_cast<size_t>(std::max(capacity, 4)), 0.0f);
        writeIndex = 0;
    }

    void reset() {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }

    void push(float sample) {
        buffer[static_cast<size_t>(writeIndex)] = sample;
        writeIndex = (writeIndex + 1) % static_cast<int>(buffer.size());
    }

    float readMs(float delayMs) const {
        const float delaySamples = static_cast<float>(0.001 * delayMs * sampleRate);
        return readSamples(delaySamples);
    }

    float readSamples(float delaySamples) const {
        const int size = static_cast<int>(buffer.size());
        float readPos = static_cast<float>(writeIndex) - delaySamples;
        while (readPos < 0.0f)
            readPos += static_cast<float>(size);

        const int index0 = static_cast<int>(readPos) % size;
        const int index1 = (index0 + 1) % size;
        const float frac = readPos - std::floor(readPos);
        return buffer[static_cast<size_t>(index0)] * (1.0f - frac) + buffer[static_cast<size_t>(index1)] * frac;
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> buffer;
    int writeIndex = 0;
};

} // namespace audio
