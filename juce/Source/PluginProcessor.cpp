#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <vector>

SynthAudioProcessor::SynthAudioProcessor() = default;
SynthAudioProcessor::~SynthAudioProcessor() = default;

const juce::String SynthAudioProcessor::getName() const { return "Audio DSP Synth"; }
bool SynthAudioProcessor::acceptsMidi() const { return true; }
bool SynthAudioProcessor::producesMidi() const { return false; }
bool SynthAudioProcessor::isMidiEffect() const { return false; }
double SynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SynthAudioProcessor::getNumPrograms() { return 1; }
int SynthAudioProcessor::getCurrentProgram() { return currentProgramIndex; }
void SynthAudioProcessor::setCurrentProgram(int index) { currentProgramIndex = index; }
const juce::String SynthAudioProcessor::getProgramName(int index) { return index == 0 ? "Default" : juce::String(); }
void SynthAudioProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    engine.prepare(sampleRate, getTotalNumOutputChannels());
    engine.setParameter("frequency", frequency);
    engine.setParameter("gain", gain);
    engine.setParameter("attack", attack);
    engine.setParameter("release", release);
    engine.setParameter("waveform", static_cast<float>(waveform));
}

void SynthAudioProcessor::releaseResources() {}

bool SynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();
    return (mainOut == juce::AudioChannelSet::mono() || mainOut == juce::AudioChannelSet::stereo());
}

void SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        if (message.isNoteOn())
            engine.noteOn();
        else if (message.isNoteOff())
            engine.noteOff();
    }

    engine.setParameter("frequency", frequency);
    engine.setParameter("gain", gain);
    engine.setParameter("attack", attack);
    engine.setParameter("release", release);
    engine.setParameter("waveform", static_cast<float>(waveform));

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    const int totalSamples = numSamples * numChannels;
    std::vector<float> interleaved(totalSamples, 0.0f);

    engine.processBlock(interleaved.data(), numSamples, numChannels);

    for (int channel = 0; channel < numChannels; ++channel)
        buffer.copyFrom(channel, 0, interleaved.data() + channel, numSamples, numChannels);
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor() { return new SynthAudioProcessorEditor(*this); }

bool SynthAudioProcessor::hasEditor() const { return true; }

void SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(frequency);
    stream.writeFloat(gain);
    stream.writeFloat(attack);
    stream.writeFloat(release);
    stream.writeInt(waveform);
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    frequency = stream.readFloat();
    gain = stream.readFloat();
    attack = stream.readFloat();
    release = stream.readFloat();
    waveform = stream.readInt();
}

void SynthAudioProcessor::setParameter(const char* name, float value)
{
    if (std::strcmp(name, "frequency") == 0)
        frequency = value;
    else if (std::strcmp(name, "gain") == 0)
        gain = value;
    else if (std::strcmp(name, "attack") == 0)
        attack = value;
    else if (std::strcmp(name, "release") == 0)
        release = value;
    else if (std::strcmp(name, "waveform") == 0)
        waveform = static_cast<int>(value);
}

float SynthAudioProcessor::getFrequency() const noexcept { return frequency; }
float SynthAudioProcessor::getGain() const noexcept { return gain; }
float SynthAudioProcessor::getAttack() const noexcept { return attack; }
float SynthAudioProcessor::getRelease() const noexcept { return release; }
int SynthAudioProcessor::getWaveform() const noexcept { return waveform; }
