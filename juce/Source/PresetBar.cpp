#include "PresetBar.h"
#include "PresetLibrary.h"
#include "RigDesign.h"

PresetBar::PresetBar() {
    label.setText("PRESET", juce::dontSendNotification);
    label.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(RigDesign::kTextSecondary));
    addAndMakeVisible(label);

    int itemId = 1;
    for (auto& preset : audio::getPresetLibrary())
        presetBox.addItem(preset.name, itemId++);
    presetBox.onChange = [this] {
        const int index = presetBox.getSelectedId() - 1;
        if (index < 0) return;
        auto& presets = audio::getPresetLibrary();
        if (index < static_cast<int>(presets.size()))
            descriptionLabel.setText(presets[static_cast<size_t>(index)].description, juce::dontSendNotification);
        if (onPresetSelected) onPresetSelected(index);
    };
    addAndMakeVisible(presetBox);

    prevButton.setButtonText("<");
    prevButton.onClick = [this] { selectRelative(-1); };
    addAndMakeVisible(prevButton);

    nextButton.setButtonText(">");
    nextButton.onClick = [this] { selectRelative(1); };
    addAndMakeVisible(nextButton);

    descriptionLabel.setFont(juce::FontOptions(11.5f));
    descriptionLabel.setColour(juce::Label::textColourId, juce::Colour(RigDesign::kTextSecondary));
    descriptionLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(descriptionLabel);

    setSelectedIndex(0);
}

void PresetBar::selectRelative(int delta) {
    const int count = presetBox.getNumItems();
    if (count == 0) return;
    const int current = presetBox.getSelectedItemIndex();
    const int next = (current + delta + count) % count;
    presetBox.setSelectedItemIndex(next, juce::sendNotificationSync);
}

void PresetBar::setSelectedIndex(int index) {
    presetBox.setSelectedId(index + 1, juce::sendNotificationSync);
}

void PresetBar::paint(juce::Graphics&) {}

void PresetBar::resized() {
    auto bounds = getLocalBounds();
    auto top = bounds.removeFromTop(16);
    label.setBounds(top);

    auto row = bounds.removeFromTop(28);
    prevButton.setBounds(row.removeFromLeft(26));
    row.removeFromLeft(4);
    nextButton.setBounds(row.removeFromRight(26));
    row.removeFromRight(4);
    presetBox.setBounds(row);

    bounds.removeFromTop(4);
    descriptionLabel.setBounds(bounds);
}
