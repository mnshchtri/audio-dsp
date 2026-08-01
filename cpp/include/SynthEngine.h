#pragma once

#include <array>
#include <cmath>

namespace audio {

class SynthEngine {
public:
    SynthEngine();
    void prepare(double sampleRate, int numChannels);
    void reset();
    void setFrequency(float frequency);
    void setWaveform(int waveformIndex);
    void setAttack(float seconds);
    void setRelease(float seconds);
    void setGain(float gain);
    void noteOn();
    void noteOff();
    void process(float* output, int numSamples, int numChannels);

private:
    enum Waveform { Sine, Saw, Square, Triangle };
    Waveform waveform = Sine;
    double sampleRate = 44100.0;
    double phase = 0.0;
    float frequency = 440.0f;
    float gain = 0.2f;
    float attack = 0.01f;
    float release = 0.2f;
    float envLevel = 0.0f;
    bool noteIsOn = false;

    float oscillatorSample();
    void updateEnvelope();
};

} // namespace audio
