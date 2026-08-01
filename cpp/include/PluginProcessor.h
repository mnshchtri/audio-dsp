#pragma once

#include "SynthEngine.h"

namespace audio {

class PluginProcessor {
public:
    PluginProcessor();
    void prepare(double sampleRate, int numChannels);
    void reset();
    void setParameter(const char* name, float value);
    void noteOn();
    void noteOff();
    void processBlock(float* output, int numSamples, int numChannels);

private:
    SynthEngine synth;
};

} // namespace audio
