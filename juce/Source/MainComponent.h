#pragma once

#include <JuceHeader.h>
#include "ChainStripComponent.h"
#include "EffectBrowserPanel.h"
#include "GuitarRigEngine.h"
#include "LevelMeter.h"
#include "ModuleEditorPanel.h"
#include "RigLookAndFeel.h"
#include <atomic>

// Live guitar-effects rack: registers directly as an AudioIODeviceCallback
// (rather than going through AudioAppComponent::getNextAudioBlock, which is
// playback-oriented and doesn't forward live input) so real input audio from
// an interface is processed through GuitarRigEngine with minimal latency.
//
// UI is a Neural-DSP-style layout: a horizontal signal-chain strip of
// pedal cards (click to select, drag to reorder, LED to bypass), a large
// focused editor for whichever pedal is selected, and a slide-in effect
// browser for adding new pedals.
class MainComponent : public juce::Component,
                      private juce::AudioIODeviceCallback
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                          float* const* outputChannelData, int numOutputChannels,
                                          int numSamples, const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    void openAudioSettings();
    void selectModule(int index);
    void setBrowserVisible(bool visible);

    juce::AudioDeviceManager deviceManager;
    audio::GuitarRigEngine engine;

    RigLookAndFeel lookAndFeel;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::TextButton audioSettingsButton;
    juce::Label inputLevelLabel;
    LevelMeter inputLevelMeter;

    juce::Label inputGainLabel;
    juce::Slider inputGainSlider;
    juce::Label outputGainLabel;
    juce::Slider outputGainSlider;

    ChainStripComponent chainStrip;
    ModuleEditorPanel moduleEditor;
    EffectBrowserPanel browserPanel;
    bool browserVisible = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
