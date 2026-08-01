#pragma once

#include <JuceHeader.h>
#include <functional>

// Neural-DSP-style preset browser: a dropdown of full-rig presets plus
// prev/next arrows to step through them, with the current preset's
// description shown underneath.
class PresetBar : public juce::Component {
public:
    PresetBar();

    std::function<void(int presetIndex)> onPresetSelected;

    void setSelectedIndex(int index);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void selectRelative(int delta);

    juce::Label label;
    juce::ComboBox presetBox;
    juce::TextButton prevButton;
    juce::TextButton nextButton;
    juce::Label descriptionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBar)
};
