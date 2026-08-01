#include "MainComponent.h"
#include "PresetLibrary.h"
#include "RigDesign.h"
#include <cstring>

MainComponent::MainComponent() : chainStrip(engine) {
    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

    titleLabel.setText("AUDIO DSP  //  GUITAR RIG", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("Build your own signal chain", juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(12.5f));
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour(RigDesign::kTextSecondary));
    addAndMakeVisible(subtitleLabel);

    presetBar.onPresetSelected = [this](int index) {
        auto& presets = audio::getPresetLibrary();
        if (index < 0 || index >= static_cast<int>(presets.size())) return;
        engine.loadPreset(presets[static_cast<size_t>(index)]);
        chainStrip.refresh();
        chainStrip.setSelectedIndex(0);
    };
    addAndMakeVisible(presetBar);

    audioSettingsButton.setButtonText("Audio Settings...");
    audioSettingsButton.onClick = [this] { openAudioSettings(); };
    addAndMakeVisible(audioSettingsButton);

    inputLevelLabel.setText("IN", juce::dontSendNotification);
    inputLevelLabel.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    inputLevelLabel.setJustificationType(juce::Justification::centred);
    inputLevelLabel.setColour(juce::Label::textColourId, juce::Colour(RigDesign::kTextSecondary));
    addAndMakeVisible(inputLevelLabel);
    addAndMakeVisible(inputLevelMeter);

    auto setupMasterKnob = [this](juce::Slider& slider, juce::Label& label, const juce::String& text,
                                   double minValue, double maxValue, double defaultValue) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::FontOptions(10.5f, juce::Font::bold));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 15);
        slider.setRange(minValue, maxValue, 0.1);
        slider.setDoubleClickReturnValue(true, defaultValue);
        slider.setValue(defaultValue, juce::dontSendNotification);
        slider.setTextValueSuffix(" dB");
        addAndMakeVisible(slider);
    };
    setupMasterKnob(inputGainSlider, inputGainLabel, "TRIM", -24.0, 24.0, 0.0);
    setupMasterKnob(outputGainSlider, outputGainLabel, "MASTER", -24.0, 24.0, 0.0);
    inputGainSlider.onValueChange = [this] { engine.setInputGainDb(static_cast<float>(inputGainSlider.getValue())); };
    outputGainSlider.onValueChange = [this] { engine.setOutputGainDb(static_cast<float>(outputGainSlider.getValue())); };

    addAndMakeVisible(chainStrip);
    addAndMakeVisible(moduleEditor);
    addChildComponent(browserPanel);

    chainStrip.onSelectionChanged = [this](int index) { selectModule(index); };
    chainStrip.onChainChanged = [this] {
        juce::MessageManager::callAsync([this] { chainStrip.refresh(); });
    };
    chainStrip.onAddRequested = [this] { setBrowserVisible(true); };

    browserPanel.onEffectChosen = [this](audio::EffectType type) {
        const size_t index = engine.addModule(type);
        chainStrip.refresh();
        chainStrip.setSelectedIndex(static_cast<int>(index));
        setBrowserVisible(false);
    };
    browserPanel.onCloseRequested = [this] { setBrowserVisible(false); };

    moduleEditor.onBypassChanged = [this](bool engaged) {
        const int index = chainStrip.getSelectedIndex();
        if (index < 0) return;
        if (auto* effect = engine.getChain().getEffect(static_cast<size_t>(index)))
            effect->setBypassed(!engaged);
        chainStrip.refresh();
    };

    selectModule(chainStrip.getSelectedIndex());

    setSize(1280, 760);

    deviceManager.initialiseWithDefaultDevices(1, 2);
    deviceManager.addAudioCallback(this);
}

MainComponent::~MainComponent() {
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void MainComponent::selectModule(int index) {
    audio::Effect* effect = index >= 0 ? engine.getChain().getEffect(static_cast<size_t>(index)) : nullptr;
    moduleEditor.showEffect(effect);
}

void MainComponent::setBrowserVisible(bool visible) {
    browserVisible = visible;
    browserPanel.setVisible(visible);
    if (visible) browserPanel.toFront(true);
    resized();
}

void MainComponent::openAudioSettings() {
    auto selector = std::make_unique<juce::AudioDeviceSelectorComponent>(
        deviceManager, 0, 2, 0, 2, false, false, true, false);
    selector->setSize(560, 440);

    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(selector.release());
    options.dialogTitle = "Audio Settings";
    options.dialogBackgroundColour = juce::Colour(RigDesign::kBackgroundBottom);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = false;
    options.launchAsync();
}

void MainComponent::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                                      float* const* outputChannelData, int numOutputChannels,
                                                      int numSamples, const juce::AudioIODeviceCallbackContext&) {
    for (int ch = 0; ch < numOutputChannels; ++ch)
        if (outputChannelData[ch] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);

    if (numInputChannels <= 0 || numOutputChannels <= 0)
        return;

    const int processChannels = juce::jmin(numOutputChannels, audio::kMaxChannels);

    for (int ch = 0; ch < processChannels; ++ch) {
        const float* inputSrc = inputChannelData[juce::jmin(ch, numInputChannels - 1)];
        if (inputSrc != nullptr && outputChannelData[ch] != nullptr)
            std::memcpy(outputChannelData[ch], inputSrc, sizeof(float) * static_cast<size_t>(numSamples));
    }

    float* channels[audio::kMaxChannels];
    for (int ch = 0; ch < processChannels; ++ch) channels[ch] = outputChannelData[ch];

    engine.process(channels, processChannels, numSamples);

    float peak = 0.0f;
    for (int ch = 0; ch < processChannels; ++ch)
        for (int i = 0; i < numSamples; ++i)
            peak = juce::jmax(peak, std::abs(channels[ch][i]));
    inputLevelMeter.pushLevel(peak);
}

void MainComponent::audioDeviceAboutToStart(juce::AudioIODevice* device) {
    const double sampleRate = device != nullptr ? device->getCurrentSampleRate() : 44100.0;
    engine.prepare(sampleRate);
    engine.reset();
}

void MainComponent::audioDeviceStopped() {
    engine.reset();
}

void MainComponent::paint(juce::Graphics& g) {
    juce::ColourGradient backgroundGradient(juce::Colour(RigDesign::kBackgroundTop), 0, 0,
                                             juce::Colour(RigDesign::kBackgroundBottom), 0,
                                             static_cast<float>(getHeight()), false);
    g.setGradientFill(backgroundGradient);
    g.fillAll();
}

void MainComponent::resized() {
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(78).reduced(20, 10);

    auto titleArea = header.removeFromLeft(header.getWidth() - 420);
    titleLabel.setBounds(titleArea.removeFromTop(28));
    subtitleLabel.setBounds(titleArea);

    auto masterArea = header;
    audioSettingsButton.setBounds(masterArea.removeFromRight(150).withSizeKeepingCentre(140, 30));

    auto meterArea = masterArea.removeFromRight(46);
    inputLevelLabel.setBounds(meterArea.removeFromTop(14));
    inputLevelMeter.setBounds(meterArea.reduced(12, 2));

    auto outputKnobArea = masterArea.removeFromRight(72);
    outputGainLabel.setBounds(outputKnobArea.removeFromTop(13));
    outputGainSlider.setBounds(outputKnobArea);

    auto inputKnobArea = masterArea.removeFromRight(72);
    inputGainLabel.setBounds(inputKnobArea.removeFromTop(13));
    inputGainSlider.setBounds(inputKnobArea);

    auto presetRow = bounds.removeFromTop(58).reduced(20, 6);
    presetBar.setBounds(presetRow.removeFromLeft(juce::jmin(480, presetRow.getWidth())));

    bounds.reduce(20, 0);
    bounds.removeFromTop(8);
    chainStrip.setBounds(bounds.removeFromTop(150));
    bounds.removeFromTop(16);
    moduleEditor.setBounds(bounds.removeFromBottom(bounds.getHeight() - 4));

    const int browserWidth = juce::jmin(340, getWidth() - 40);
    browserPanel.setBounds(getWidth() - (browserVisible ? browserWidth : 0), 0, browserWidth, getHeight());
}
