#pragma once

#include <JuceHeader.h>
#include "../cpp/include/PluginProcessor.h"

class SynthAudioProcessorEditor;

class SynthAudioProcessor : public juce::AudioProcessor
{
public:
    SynthAudioProcessor();
    ~SynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setParameter(const char* name, float value);
    float getFrequency() const noexcept;
    float getGain() const noexcept;
    float getAttack() const noexcept;
    float getRelease() const noexcept;
    int getWaveform() const noexcept;

private:
    audio::PluginProcessor engine;
    double currentSampleRate = 44100.0;
    float frequency = 440.0f;
    float gain = 0.3f;
    float attack = 0.01f;
    float release = 0.2f;
    int waveform = 0;
    bool shouldPlayNote = false;
    int currentProgramIndex = 0;
};
