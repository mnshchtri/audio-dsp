#include "EffectBrowserPanel.h"
#include "RigDesign.h"

EffectBrowserPanel::EffectTile::EffectTile(audio::EffectType t, const juce::String& title, juce::Colour accentColour)
    : type(t), name(title), accent(accentColour) {}

void EffectBrowserPanel::EffectTile::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    juce::ColourGradient bg(juce::Colour(RigDesign::kPanelColourAlt).brighter(hovered ? 0.06f : 0.0f),
                             bounds.getX(), bounds.getY(),
                             juce::Colour(RigDesign::kPanelColour), bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(bg);
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(hovered ? accent : juce::Colour(RigDesign::kBorderColour));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 8.0f, hovered ? 1.6f : 1.0f);

    auto dot = juce::Rectangle<float>(9.0f, 9.0f).withCentre({ bounds.getX() + 16.0f, bounds.getCentreY() });
    g.setColour(accent);
    g.fillEllipse(dot);

    g.setColour(juce::Colour(RigDesign::kTextPrimary));
    g.setFont(juce::FontOptions(14.0f, hovered ? juce::Font::bold : juce::Font::plain));
    g.drawFittedText(name, bounds.withTrimmedLeft(32.0f).toNearestInt(), juce::Justification::centredLeft, 1);

    if (hovered) {
        g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
        g.drawFittedText("+", bounds.withTrimmedRight(12.0f).toNearestInt(), juce::Justification::centredRight, 1);
    }
}

EffectBrowserPanel::EffectBrowserPanel() {
    titleLabel.setText("ADD EFFECT", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    closeButton.setButtonText(juce::CharPointer_UTF8("\xc3\x97"));
    closeButton.onClick = [this] { if (onCloseRequested) onCloseRequested(); };
    addAndMakeVisible(closeButton);

    viewport.setViewedComponent(&listContent, false);
    viewport.setScrollBarsShown(true, false);
    addAndMakeVisible(viewport);

    juce::String lastCategory;
    for (auto& info : audio::getEffectTypeRegistry()) {
        const juce::String category(info.category);
        if (category != lastCategory) {
            auto label = std::make_unique<juce::Label>();
            label->setText(category.toUpperCase(), juce::dontSendNotification);
            label->setFont(juce::FontOptions(12.0f, juce::Font::bold));
            label->setColour(juce::Label::textColourId, RigDesign::colourForCategory(category).brighter(0.2f));
            listContent.addAndMakeVisible(*label);
            categoryLabels.add(label.release());
            lastCategory = category;
        }

        auto tile = std::make_unique<EffectTile>(info.type, info.name, RigDesign::colourForCategory(category));
        tile->onClick = [this, type = info.type] { if (onEffectChosen) onEffectChosen(type); };
        listContent.addAndMakeVisible(*tile);
        tiles.add(tile.release());
    }
}

void EffectBrowserPanel::paint(juce::Graphics& g) {
    g.setColour(juce::Colour(0xff1a1b20).withAlpha(0.98f));
    g.fillRect(getLocalBounds());
    g.setColour(juce::Colour(RigDesign::kBorderColour));
    g.drawLine(0.0f, 0.0f, 0.0f, static_cast<float>(getHeight()), 1.5f);
}

void EffectBrowserPanel::resized() {
    auto bounds = getLocalBounds().reduced(16);
    auto header = bounds.removeFromTop(30);
    closeButton.setBounds(header.removeFromRight(30));
    titleLabel.setBounds(header);
    bounds.removeFromTop(10);

    viewport.setBounds(bounds);

    int y = 4;
    const int contentWidth = bounds.getWidth() - viewport.getScrollBarThickness() - 4;
    size_t tileIndex = 0;
    juce::String lastCategory;
    size_t categoryIndex = 0;
    for (auto& info : audio::getEffectTypeRegistry()) {
        const juce::String category(info.category);
        if (category != lastCategory) {
            if (categoryIndex < static_cast<size_t>(categoryLabels.size())) {
                categoryLabels[static_cast<int>(categoryIndex)]->setBounds(4, y, contentWidth, 20);
                ++categoryIndex;
            }
            y += 26;
            lastCategory = category;
        }
        if (tileIndex < static_cast<size_t>(tiles.size())) {
            tiles[static_cast<int>(tileIndex)]->setBounds(4, y, contentWidth, 40);
            ++tileIndex;
        }
        y += 46;
    }
    listContent.setSize(contentWidth + 8, y + 10);
}
