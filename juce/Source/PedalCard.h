#pragma once

#include <JuceHeader.h>
#include <functional>

// A compact card styled as a miniature stompbox enclosure, representing one
// pedal/module in the horizontal signal chain strip: hammertone paint in
// the category colour, a chrome footswitch that bypasses the module, an LED
// jewel, and a remove button. Click-to-select / drag-to-reorder is handled
// via manual hit-testing (rather than child Button components) so drag
// gestures aren't swallowed by a child component.
class PedalCard : public juce::Component {
public:
    PedalCard(const juce::String& title, const juce::String& categoryLabel, juce::Colour accentColour);

    void setSelected(bool shouldBeSelected);
    bool isSelected() const { return selected; }
    void setEngaged(bool engaged);
    bool isEngaged() const { return engagedState; }

    std::function<void()> onSelect;
    std::function<void(bool)> onBypassToggle;
    std::function<void()> onRemove;
    std::function<void(const juce::MouseEvent&)> onDragStart;
    std::function<void(const juce::MouseEvent&)> onDragMove;
    std::function<void(const juce::MouseEvent&)> onDragEnd;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent&) override { hovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&) override { hovered = false; repaint(); }

private:
    juce::Rectangle<float> getSwitchBounds() const;
    juce::Rectangle<float> getLedJewelBounds() const;
    juce::Rectangle<float> getRemoveBounds() const;

    juce::String moduleTitle;
    juce::String categoryText;
    juce::Colour accent;
    bool selected = false;
    bool engagedState = true;
    bool hovered = false;
    bool dragging = false;
};
