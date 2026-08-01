#include "PluginEditor.h"

SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& processor)
    : juce::AudioProcessorEditor(&processor), audioProcessor(processor)
{
    titleLabel.setText("Audio DSP Synth", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    addAndMakeVisible(frequencyLabel);
    frequencySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 24);
    frequencySlider.setRange(20.0, 2000.0, 1.0);
    frequencySlider.setValue(audioProcessor.getFrequency());
    frequencySlider.addListener(this);
    addAndMakeVisible(frequencySlider);

    gainLabel.setText("Gain", juce::dontSendNotification);
    addAndMakeVisible(gainLabel);
    gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 24);
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setValue(audioProcessor.getGain());
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);

    attackLabel.setText("Attack", juce::dontSendNotification);
    addAndMakeVisible(attackLabel);
    attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 24);
    attackSlider.setRange(0.001, 1.0, 0.001);
    attackSlider.setValue(audioProcessor.getAttack());
    attackSlider.addListener(this);
    addAndMakeVisible(attackSlider);

    releaseLabel.setText("Release", juce::dontSendNotification);
    addAndMakeVisible(releaseLabel);
    releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 24);
    releaseSlider.setRange(0.001, 2.0, 0.001);
    releaseSlider.setValue(audioProcessor.getRelease());
    releaseSlider.addListener(this);
    addAndMakeVisible(releaseSlider);

    waveformLabel.setText("Waveform", juce::dontSendNotification);
    addAndMakeVisible(waveformLabel);
    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Saw", 2);
    waveformSelector.addItem("Square", 3);
    waveformSelector.addItem("Triangle", 4);
    waveformSelector.setSelectedId(audioProcessor.getWaveform() + 1);
    waveformSelector.addListener(this);
    addAndMakeVisible(waveformSelector);

    setSize(520, 320);
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor() = default;

void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 2);
}

void SynthAudioProcessorEditor::resized()
{
    const int margin = 16;
    const int labelHeight = 24;
    const int sliderHeight = 32;
    const int yStep = sliderHeight + 24;
    int y = margin;

    titleLabel.setBounds(margin, y, getWidth() - margin * 2, labelHeight);
    y += labelHeight + margin;

    frequencyLabel.setBounds(margin, y, 100, labelHeight);
    frequencySlider.setBounds(margin + 110, y, getWidth() - margin * 2 - 110, sliderHeight);
    y += yStep;

    gainLabel.setBounds(margin, y, 100, labelHeight);
    gainSlider.setBounds(margin + 110, y, getWidth() - margin * 2 - 110, sliderHeight);
    y += yStep;

    attackLabel.setBounds(margin, y, 100, labelHeight);
    attackSlider.setBounds(margin + 110, y, getWidth() - margin * 2 - 110, sliderHeight);
    y += yStep;

    releaseLabel.setBounds(margin, y, 100, labelHeight);
    releaseSlider.setBounds(margin + 110, y, getWidth() - margin * 2 - 110, sliderHeight);
    y += yStep;

    waveformLabel.setBounds(margin, y, 100, labelHeight);
    waveformSelector.setBounds(margin + 110, y, getWidth() - margin * 2 - 110, sliderHeight);
}

void SynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &frequencySlider)
        audioProcessor.setParameter("frequency", static_cast<float>(frequencySlider.getValue()));
    else if (slider == &gainSlider)
        audioProcessor.setParameter("gain", static_cast<float>(gainSlider.getValue()));
    else if (slider == &attackSlider)
        audioProcessor.setParameter("attack", static_cast<float>(attackSlider.getValue()));
    else if (slider == &releaseSlider)
        audioProcessor.setParameter("release", static_cast<float>(releaseSlider.getValue()));
}

void SynthAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &waveformSelector)
        audioProcessor.setParameter("waveform", static_cast<float>(waveformSelector.getSelectedId() - 1));
}
