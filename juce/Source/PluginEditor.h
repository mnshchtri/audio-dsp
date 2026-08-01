#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class SynthAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   private juce::Slider::Listener,
                                   private juce::ComboBox::Listener
{
public:
    explicit SynthAudioProcessorEditor(SynthAudioProcessor& processor);
    ~SynthAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    SynthAudioProcessor& audioProcessor;
    juce::Slider frequencySlider;
    juce::Slider gainSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::ComboBox waveformSelector;
    juce::Label titleLabel;
    juce::Label waveformLabel;
    juce::Label frequencyLabel;
    juce::Label gainLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};
