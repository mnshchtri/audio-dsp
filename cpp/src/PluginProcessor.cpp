#include "PluginProcessor.h"
#include <cstring>

namespace audio {

PluginProcessor::PluginProcessor() = default;

void PluginProcessor::prepare(double sampleRate, int numChannels) {
    synth.prepare(sampleRate, numChannels);
}

void PluginProcessor::reset() {
    synth.reset();
}

void PluginProcessor::setParameter(const char* name, float value) {
    if (std::strcmp(name, "frequency") == 0) {
        synth.setFrequency(value);
    } else if (std::strcmp(name, "gain") == 0) {
        synth.setGain(value);
    } else if (std::strcmp(name, "attack") == 0) {
        synth.setAttack(value);
    } else if (std::strcmp(name, "release") == 0) {
        synth.setRelease(value);
    } else if (std::strcmp(name, "waveform") == 0) {
        synth.setWaveform(static_cast<int>(value));
    }
}

void PluginProcessor::noteOn() {
    synth.noteOn();
}

void PluginProcessor::noteOff() {
    synth.noteOff();
}

void PluginProcessor::processBlock(float* output, int numSamples, int numChannels) {
    synth.process(output, numSamples, numChannels);
}

} // namespace audio
