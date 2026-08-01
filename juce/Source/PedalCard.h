#pragma once

#include <JuceHeader.h>
#include <functional>
#include <vector>

// A compact card styled as a miniature stompbox enclosure, representing one
// pedal/module in the horizontal signal chain strip. Unlike a generic
// placeholder, it shows the pedal's *actual* controls in miniature (one dial
// per real parameter, at its real value) so each pedal reads as its own
// piece of hardware rather than an identical box with a different label -
// a 1-knob gate looks like a 1-knob gate, a 5-knob compressor looks bigger
// and busier. Click-to-select / drag-to-reorder is handled via manual
// hit-testing (rather than child Button components) so drag gestures
// aren't swallowed by a child component.
class PedalCard : public juce::Component {
public:
    struct MiniKnob {
        juce::String label;
        float normalized = 0.5f; // 0..1 position within the parameter's range
    };

    PedalCard(const juce::String& title, const juce::String& categoryLabel, juce::Colour accentColour);

    void setSelected(bool shouldBeSelected);
    bool isSelected() const { return selected; }
    void setEngaged(bool engaged);
    bool isEngaged() const { return engagedState; }
    void setKnobs(std::vector<MiniKnob> newKnobs);

    // Width this card would like to occupy, based on how many controls it's
    // showing - more knobs, a physically bigger-looking pedal.
    int getPreferredWidth() const;
    static constexpr int kPreferredHeight = 156;

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
    juce::Rectangle<float> getKnobRowBounds() const;

    juce::String moduleTitle;
    juce::String categoryText;
    juce::Colour accent;
    bool selected = false;
    bool engagedState = true;
    bool hovered = false;
    bool dragging = false;
    std::vector<MiniKnob> knobs;
};
