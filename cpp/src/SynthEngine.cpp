#include "SynthEngine.h"

namespace audio {

SynthEngine::SynthEngine() = default;

void SynthEngine::prepare(double sampleRateIn, int numChannels) {
    sampleRate = sampleRateIn;
    phase = 0.0;
    envLevel = 0.0f;
    noteIsOn = false;
    (void)numChannels;
}

void SynthEngine::reset() {
    phase = 0.0;
    envLevel = 0.0f;
    noteIsOn = false;
}

void SynthEngine::setFrequency(float frequencyIn) {
    frequency = frequencyIn;
}

void SynthEngine::setWaveform(int waveformIndex) {
    waveform = static_cast<Waveform>(std::clamp(waveformIndex, 0, 3));
}

void SynthEngine::setAttack(float seconds) {
    attack = std::max(0.001f, seconds);
}

void SynthEngine::setRelease(float seconds) {
    release = std::max(0.001f, seconds);
}

void SynthEngine::setGain(float gainIn) {
    gain = std::clamp(gainIn, 0.0f, 1.0f);
}

void SynthEngine::noteOn() {
    noteIsOn = true;
}

void SynthEngine::noteOff() {
    noteIsOn = false;
}

float SynthEngine::oscillatorSample() {
    const double increment = (2.0 * M_PI * frequency) / sampleRate;
    float value = 0.0f;

    switch (waveform) {
        case Sine:
            value = static_cast<float>(std::sin(phase));
            break;
        case Saw:
            value = static_cast<float>((2.0 * (phase / (2.0 * M_PI))) - 1.0);
            break;
        case Square:
            value = phase < M_PI ? 1.0f : -1.0f;
            break;
        case Triangle: {
            float normalized = static_cast<float>(phase / (2.0 * M_PI));
            value = 4.0f * std::abs(normalized - 0.5f) - 1.0f;
            break;
        }
    }

    phase += increment;
    if (phase >= 2.0 * M_PI)
        phase -= 2.0 * M_PI;

    return value;
}

void SynthEngine::updateEnvelope() {
    const float attackIncrement = 1.0f / static_cast<float>(std::max(1.0, attack * sampleRate));
    const float releaseIncrement = 1.0f / static_cast<float>(std::max(1.0, release * sampleRate));

    if (noteIsOn) {
        envLevel = std::min(1.0f, envLevel + attackIncrement);
    } else {
        envLevel = std::max(0.0f, envLevel - releaseIncrement);
    }
}

void SynthEngine::process(float* output, int numSamples, int numChannels) {
    for (int sample = 0; sample < numSamples; ++sample) {
        updateEnvelope();
        const float osc = oscillatorSample();
        const float value = osc * envLevel * gain;

        for (int channel = 0; channel < numChannels; ++channel) {
            output[sample * numChannels + channel] = value;
        }
    }
}

} // namespace audio
