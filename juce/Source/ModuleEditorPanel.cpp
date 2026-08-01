#include "ModuleEditorPanel.h"
#include "PedalTexture.h"
#include "RigDesign.h"
#include "RigLookAndFeel.h"

ModuleEditorPanel::ModuleEditorPanel() {
    nameLabel.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    nameLabel.setColour(juce::Label::textColourId, juce::Colour(RigDesign::kTextPrimary));
    addAndMakeVisible(nameLabel);

    categoryLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    addAndMakeVisible(categoryLabel);

    bypassButton.setButtonText("ON");
    bypassButton.setToggleState(true, juce::dontSendNotification);
    bypassButton.onClick = [this] {
        if (onBypassChanged) onBypassChanged(bypassButton.getToggleState());
    };
    addAndMakeVisible(bypassButton);

    nameLabel.setText("No effect selected", juce::dontSendNotification);
    categoryLabel.setText("Add a pedal to get started", juce::dontSendNotification);
    bypassButton.setVisible(false);
}

void ModuleEditorPanel::showEffect(audio::Effect* effect) {
    knobs.clear();
    hasEffect = effect != nullptr;
    bypassButton.setVisible(hasEffect);

    if (!hasEffect) {
        nameLabel.setText("No effect selected", juce::dontSendNotification);
        categoryLabel.setText("Add a pedal to get started", juce::dontSendNotification);
        accent = juce::Colour(0xffB0B4BB);
        repaint();
        resized();
        return;
    }

    accent = RigDesign::colourForCategory(effect->category());
    nameLabel.setText(effect->name(), juce::dontSendNotification);
    categoryLabel.setText(juce::String(effect->category()).toUpperCase(), juce::dontSendNotification);
    categoryLabel.setColour(juce::Label::textColourId, accent);
    bypassButton.setToggleState(!effect->isBypassed(), juce::dontSendNotification);

    for (auto& param : effect->getParameters()) {
        Knob knob;
        knob.slider = std::make_unique<juce::Slider>();
        knob.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob.slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 78, 18);
        knob.slider->setColour(juce::Slider::rotarySliderFillColourId, accent);
        const double step = (param.maxValue - param.minValue) > 5.0 ? 0.1 : 0.001;
        knob.slider->setRange(param.minValue, param.maxValue, step);
        knob.slider->setDoubleClickReturnValue(true, param.defaultValue);
        knob.slider->setValue(param.defaultValue, juce::dontSendNotification);
        if (!param.suffix.empty()) knob.slider->setTextValueSuffix(param.suffix);
        auto setValue = param.setValue;
        knob.slider->onValueChange = [setValue, sliderPtr = knob.slider.get()] {
            setValue(static_cast<float>(sliderPtr->getValue()));
        };
        setValue(param.defaultValue);

        knob.label = std::make_unique<juce::Label>();
        knob.label->setText(param.label, juce::dontSendNotification);
        knob.label->setJustificationType(juce::Justification::centred);
        knob.label->setFont(juce::FontOptions(13.0f, juce::Font::bold));

        addAndMakeVisible(*knob.slider);
        addAndMakeVisible(*knob.label);
        knobs.push_back(std::move(knob));
    }

    repaint();
    resized();
}

void ModuleEditorPanel::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    if (hasEffect) {
        juce::ColourGradient glow(accent.withAlpha(0.08f), bounds.getCentreX(), bounds.getY(),
                                   juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getBottom(), false);
        g.setGradientFill(glow);
        g.fillRect(bounds);
    }

    // Faceplate: dark brushed panel (kept low-saturation so a screen full of
    // knobs stays legible) with a faint hammertone overlay for texture, and
    // corner screws to read as one big pedal rather than a flat card.
    auto panelBounds = bounds.reduced(1.0f);
    juce::ColourGradient panelGrad(juce::Colour(RigDesign::kPanelColourAlt), panelBounds.getX(), panelBounds.getY(),
                                    juce::Colour(RigDesign::kPanelColour), panelBounds.getX(), panelBounds.getBottom(), false);
    g.setGradientFill(panelGrad);
    g.fillRoundedRectangle(panelBounds, 14.0f);

    if (hasEffect) {
        g.saveState();
        juce::Path clip;
        clip.addRoundedRectangle(panelBounds, 14.0f);
        g.reduceClipRegion(clip);
        g.setTiledImageFill(PedalTexture::hammertoneTexture(), 0, 0, 0.5f);
        g.fillRect(panelBounds);
        g.restoreState();
    }

    g.setColour(hasEffect ? accent.withAlpha(0.7f) : juce::Colour(RigDesign::kBorderColour));
    g.drawRoundedRectangle(panelBounds.reduced(0.5f), 14.0f, hasEffect ? 1.6f : 1.0f);

    if (hasEffect) {
        const float inset = 16.0f;
        PedalTexture::drawScrew(g, { panelBounds.getX() + inset, panelBounds.getY() + inset }, 4.2f);
        PedalTexture::drawScrew(g, { panelBounds.getRight() - inset, panelBounds.getY() + inset }, 4.2f);
        PedalTexture::drawScrew(g, { panelBounds.getX() + inset, panelBounds.getBottom() - inset }, 4.2f);
        PedalTexture::drawScrew(g, { panelBounds.getRight() - inset, panelBounds.getBottom() - inset }, 4.2f);

        auto header = bounds.reduced(30.0f, 22.0f).removeFromTop(3.0f);
        g.setColour(accent);
        g.fillRoundedRectangle(header.withWidth(56.0f), 1.5f);
    }
}

void ModuleEditorPanel::resized() {
    auto bounds = getLocalBounds().reduced(28, 22);

    auto header = bounds.removeFromTop(46);
    auto textArea = header.removeFromLeft(header.getWidth() - 100);
    nameLabel.setBounds(textArea.removeFromTop(30));
    categoryLabel.setBounds(textArea);
    bypassButton.setBounds(header.withSizeKeepingCentre(90, 34));

    if (knobs.empty()) return;

    bounds.removeFromTop(20);
    const int knobCount = static_cast<int>(knobs.size());
    const int maxPerRow = juce::jmax(1, juce::jmin(knobCount, bounds.getWidth() / 130));
    const int numRows = (knobCount + maxPerRow - 1) / maxPerRow;
    const int rowHeight = juce::jmin(180, bounds.getHeight() / juce::jmax(1, numRows));

    int index = 0;
    for (int row = 0; row < numRows; ++row) {
        const int remaining = knobCount - index;
        const int thisRowCount = juce::jmin(maxPerRow, remaining);
        const int knobWidth = bounds.getWidth() / thisRowCount;
        auto rowArea = bounds.removeFromTop(rowHeight);
        for (int i = 0; i < thisRowCount; ++i) {
            auto area = rowArea.removeFromLeft(knobWidth);
            auto labelArea = area.removeFromTop(20);
            knobs[static_cast<size_t>(index)].label->setBounds(labelArea);
            knobs[static_cast<size_t>(index)].slider->setBounds(area.reduced(10));
            ++index;
        }
    }
}
